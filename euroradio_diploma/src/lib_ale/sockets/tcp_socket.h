#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "alepkt_parser.h"
#include "vtcpsocket.h"
#include "vtimer.h"

// Класс TCP_Socket предназначен для управления и индикации отдельного TCP-соединения
class AleTcpSocket
{

public:

    // Параметр - тип ошибки сокета
    enum SOCKET_ERROR_TYPE
    {
        ERROR_BROKEN_PIPE,
        ERROR_CONNECTION_REFUSED
    };

    // Параметр - тип сокета
    enum SOCKET_TYPE
    {
        CLIENT,
        SERVER,
        WAIT_SERVER
    };

    enum SOCKET_STATUS
    {
        IDLE,
        WAIT_FOR_CONNECT,   // Waiting for TCP connection
        OK                  // Connect established
    };

    // Параметр - канал TCP соединения
    enum NR_FLAG : uint8_t
    {
        N = 1,  // Normal
        R = 0   // Redundant
    };

//==================== СИГНАЛЫ ===========================================================

    VSignal<AlepktAu1>     received_au1;   // Получен ALEPKT AU1
    VSignal<AlepktAu2>     received_au2;   // Получен ALEPKT AU2
    VSignal<AlepktHeader>  received_dt;    // Получен ALEPKT DT
    VSignal<AlepktHeader>  received_di;    // Получен ALEPKT DI

    VSignal<SOCKET_ERROR_TYPE>  socket_error;           // Ошибка в сокете
    VSignal<>                   socket_disconnected;    // Сокет отключился
    VSignal<>                   socket_connected;       // Сокет подключен

    VSignal<>           wait_socket_fail;           // Ошибка в ожидающем сокете
    VSignal<AlepktAu1>  wait_socket_received_AU1;   // Ожидающий сокет получил AU1
    VSignal<>           succsessful_send;

// ========================================================================================

    // Конструктор клиентского сокета
    AleTcpSocket(SOCKET_TYPE type, const uint32_t data_wait_period_ms);

    // Конструктор серверного ожидающего сокета
    AleTcpSocket(SOCKET_TYPE type, VTcpSocket::Peer& peer);

    SOCKET_STATUS getSocketStatus();
    void setSocketStatus(SOCKET_STATUS status);

    // Изменить тип сокета
    void changeSocketType( const SOCKET_TYPE type );

    // Метод для настройки подключенного TCP-соединения и включения опции KeepAlive
    void tuneReadyRead();
    void tuneSocketSignals();
    void tuneWaitServerSignals();

    void unlinkOwnSignals();    
    void unlinkAllSignals();
    void unlinkWaitServerSignals();

    void connect(vbyte_buffer ip, uint16_t _port);
    void disconnect();

    void send(vbyte_buffer alepkt);

    inline void increaseNumOfDisconnections() { _num_of_disconnections++; }

    const std::shared_ptr<VTcpSocket> getSocket() const;

private:

    std::shared_ptr<VTcpSocket> _socket = nullptr;
    AlepktParser                _parser;
    uint16_t                    _num_of_disconnections = 0;

    VTimer                      _data_wait_timer;
    std::chrono::milliseconds   _data_wait_period = std::chrono::milliseconds(4000);

    SOCKET_TYPE     _socket_type;
    SOCKET_STATUS   _socket_status = IDLE;

    std::string     _ip;
    uint16_t        _port;
};

#endif // TCP_SOCKET_H
