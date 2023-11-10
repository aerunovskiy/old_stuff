#ifndef SOCKET_PAIR_H
#define SOCKET_PAIR_H

#include <memory>
#include <tcp_socket.h>

// Класс предназначен для реализации логики взаимодействия двух дублирующих
// TCP - соединений на уровне протокола ALE (SS-098 6.6.2).
class SocketPair
{

public:

    void initSocketPair( int pair_id );

    // Перечень возможных статусов сокета
    enum STATUS
    {
        WAIT_TCP,   // Waiting TCP connection
        WAIT_AU2,   // Ожидает ALEPKT AU2
        OK,         // Соединение установлено, сокет готов получать ALEPKT DT
        DISC        // Соединение отсутствует
    };

    enum PAIR_TYPE
    {
        CLIENT,
        SERVER,
        UNDEFINED
    };

    static const int max_tsn = 65535; // Максимальный размер TSN

//======================= СИГНАЛЫ ========================================================
    VSignal<AlepktAu1,      AleTcpSocket*>  received_au1; // Получен ALEPKT AU1
    VSignal<AlepktAu2,      uint16_t>       received_au2; // Получен ALEPKT AU2
    VSignal<AlepktHeader,   uint16_t, bool> received_dt;  // Получен ALEPKT DT; является ли принявший данные канал основным
    VSignal<AlepktHeader,   uint16_t>       received_di;  // Получен ALEPKT DI
    VSignal<                uint16_t>       sock_disc;    // TCP-сокет отключился

    VSignal<AleTcpSocket::NR_FLAG>          client_sock_connected; // Клиентский TCP-сокет подключился

    VSignal<>                               pair_cleared; // Пара очищена
    VSignal<bool>                           active_channel;
//========================================================================================

    // Установка TCEPID для пары сокетов
    void setId( int pair_id );

    void setSocketWaitPeriod( const uint32_t socket_wait_period );

    // Инициализация сокета в паре сокетов
    void socketInit( AleTcpSocket::NR_FLAG nr );

    // Вставка сокета в пару сокетов
    void inputSock( AleTcpSocket::NR_FLAG            nr,
                    std::shared_ptr<AleTcpSocket>   sock,
                    uint32_t                        peer_etcsid );

    static void parseSingleAddress( std::string::const_iterator&    p,
                                    std::string::const_iterator&    end_it,
                                    vbyte_buffer&                   ip,
                                    uint16_t&                       port    );

    // Метод для получения IP и порта из строки с адресом сети
    static void parseNetworkAddress(    const std::string&  _network_address,
                                        vbyte_buffer&       ip1,
                                        vbyte_buffer&       ip2,
                                        uint16_t&           port1,
                                        uint16_t&           port2           );

    void initNetworkAddress( const std::string& _network_address );

    // Подключение пары сокетов к определенному адресу
    void connect( const std::string& _network_address );

    // Очистка пары сокетов
    void clearPair();

    // Проверка, свободна ли пара сокетов
    bool isVacant();

    // Проверка, свободен ли сокет в паре
    bool isSockVacant(AleTcpSocket::NR_FLAG fl);

    // Проверка, существует ли сокет в паре
    bool isSockExists(AleTcpSocket::NR_FLAG fl);

    // Отправка пакета ALE по определенному каналу
    void send(AleTcpSocket::NR_FLAG fl, vbyte_buffer alepkt);

    // Параметр - TCEPID пары
    int pair_id;

    // Параметр - ETCSID подключенной к паре сущности
    int peer_etcsid = -1;

    // Параметр - TSN для Normal и Redundant каналов
    uint16_t tsn[2];

    // Установка статуса пары сокетов
    void setStatus(STATUS st);

    // Получение статуса пары сокетов
    const STATUS  getStatus();

    // Метод для получения указателя на vtcp_socket (vlibs)
    const std::shared_ptr<VTcpSocket> getSocket(AleTcpSocket::NR_FLAG nr) const;

    // Метод для получения указателя на TCP_Socket
    const std::shared_ptr<AleTcpSocket> getTCPSocket(AleTcpSocket::NR_FLAG nr) const;

    struct vsocket_address
    {
        VIpAddress address;
        uint16_t port;
    };

private:

    PAIR_TYPE _pair_type = UNDEFINED;

    // Хранимая пара TCP_Socket (N и R каналы)
    std::shared_ptr<AleTcpSocket> _socket[2];

    // Параметр - Последний полученный TSN для пары сокетов
    int _last_tsn = -1;

    // Параметр - текущий статус соединения пары сокетов
    STATUS  _status = DISC;

    uint32_t _socket_wait_period_ms = 4000;

    vsocket_address _network_address[2];

    VTimer _reconnect_timer[2];

    void onNormalSuccessfulSend   ();
    void onRedundantSuccessfulSend();
};

#endif // SOCKET_PAIR_H
