#pragma once

#include <map>
#include <vsettings.h>

#include "basic_ale.h"
#include "socket_pair.h"
#include "tcp_socket.h"
#include "vtcpserver.h"
#include "vlog.h"

// Класс ALE реализует логику работы слоя Adaptation Layer Entity (SS-098 Глава 6)
class Ale : public BasicAle
{

public:

    class WaitUnit
    {
    public:
        // Счётчик для ID WaitUnit
        static uint16_t counter;

        enum WAIT_UNIT_STATUS
        {
            WAITING,
            OUTDATED
        };

        VSignal <uint16_t, AlepktAu1>    wait_unit_received_au1;
        VSignal <uint16_t>               wait_unit_error;

        void turnOff()
        {
            _wait_timer.stop();
            _wait_socket->disconnect();
            wait_unit_error(_id);
        }

        void startTimer()
        {
            _wait_timer.start(std::chrono::microseconds(_wait_time));
        }

        std::shared_ptr<AleTcpSocket> getTcpSocket()
        {
            return _wait_socket;
        }

        const WAIT_UNIT_STATUS getStatus()
        {
            return _status;
        }

        void setStatus( WAIT_UNIT_STATUS status)
        {
            this->_status = status;
        }

        WaitUnit( std::shared_ptr<AleTcpSocket> sock,
                  int                           wait_time )
                :   _id(counter++),
                    _wait_time(wait_time),
                    _status(WAITING)
        {
            _wait_socket = std::move(sock);
            sock.reset();

            _wait_timer.timeout = [this]()
            {
                turnOff();
            };

            _wait_socket->wait_socket_received_AU1.connect([this](AlepktAu1 au1)
            {
                _wait_timer.stop();

                this->_wait_socket->socket_error.disconnect_all();
                this->_wait_socket->socket_disconnected.disconnect_all();
                this->_wait_socket->wait_socket_fail.disconnect_all();
                this->_wait_timer.timeout.unlink();

                wait_unit_received_au1(this->_id, au1);
            });

            _wait_socket->wait_socket_fail.connect([this]()
            {
                turnOff();
            });

            _wait_socket->socket_error.connect([this](AleTcpSocket::SOCKET_ERROR_TYPE error_type)
            {
                vfatal << "Ошибка в ожидающем сокете (ID = " << this->_id << ") #" << error_type;
                turnOff();
            });

            _wait_socket->socket_disconnected.connect([this]()
            {
                turnOff();
            });
        }

    private:

        WAIT_UNIT_STATUS                _status;
        uint16_t                        _id;
        std::shared_ptr<AleTcpSocket>   _wait_socket = nullptr;
        VTimer                          _wait_timer;
        int                             _wait_time = 0;
    };

    Ale( const vsettings &settings );

    ~Ale();

    // Инициализация серверных и клиентских пар
    void initPairs();

    bool isServerPair(uint16_t tcepid);
    bool isClientPair(uint16_t tcepid);

    SocketPair* getPair(uint16_t tcepid);

    int t_wait; // Время ожидания ответа (microsec)

    // Связь ETCSID <-> NetworkAddress
    std::map<uint32_t, std::string>     etcsid_address_book;

    // Связь ETCSID <-> ETY
    std::map<uint32_t, uint8_t>         etcsid_type_book;

//========================================================================================

    // ETCSID, которому принадлежит TSAP
    uint32_t etcsid;

    // Запрос на установление соединения
    void connectRequest      (  uint32_t            address_type,
                                const std::string&  network_address,
                                uint32_t            called_etcsid,
                                uint32_t            calling_etcsid,
                                uint8_t             application_type,
                                uint8_t             q_of_s,
                                const std::string&  user_data           ) override;

    // Ответ на запрос на установление соединения
    void connectResponse     (  uint16_t            tcepida,
                                uint32_t            responding_etcsid,
                                const std::string&  user_data           ) override;

    // Запрос на разрыв соединения
    void disconnectRequest   (  uint16_t            tcepidxmt,
                                const std::string&  user_data   ) override;

    // Запрос на передачу данных
    void dataRequest         (  uint16_t            tcepidxmt,
                                const std::string&  user_data   ) override;

    // Возвращает соответствующий адрес по ETCSID
    const std::string& getNetworkAddress(uint32_t responding_etcsid) override;

    // Установка TSN в сокет-пару с TCEPID
    void setTsn(uint16_t tcepid, uint16_t tsn);

private:

    VSignal<AlepktAu1, AleTcpSocket*> _au1_buf_received;

//===================== ДЕЙСТВИЯ ПРИ ПОЛУЧЕНИИ СИГНАЛОВ ==================================
    void onAu1Received        (AlepktAu1     au1,   std::shared_ptr<AleTcpSocket> sock );
    void onAu2Received        (AlepktAu2     au2,   uint16_t    TCEPID                 );
    void onDtReceived         (AlepktHeader  dt,    uint16_t    TCEPID, bool channel   );
    void onDiReceived         (AlepktHeader  di,    uint16_t    TCEPID                 );

    void onSockDisconnected   ( uint16_t TCEPID );

    void onPeerConnected      ( VTcpSocket::Peer peer );

    void onChannelActive      (bool channel);

    void onClientNormalConnected();
    void onClientRedundantConnected();
//========================================================================================

    // Настройка серверного сокета при подключении сущности-пира
    void tuneSocket( WaitUnit* wait_unit );

    SocketPair _client_pair;                                // Пара клиентских сокетов
    SocketPair _server_pair[max_num_of_server_connections]; // Пары серверных сокетов

    // Один сервер на один TSAP (TODO: сделать компактнее)
    VTcpServer _server[2];

    // Запуск серверов на прослушивание портов
    void waitForConnect();

    void initGarbageCollector();

    // Буфер, содержащий серверные сокеты, для которых пока не найдено подходящей пары,
    // либо для которых еще не пришёл ALEPKT AU1/DI
    std::vector< std::shared_ptr<WaitUnit> > _sock_buf;

    VTimer _garbage_buf_collector;

    uint32_t _socket_wait_period_ms = 4000;
};
