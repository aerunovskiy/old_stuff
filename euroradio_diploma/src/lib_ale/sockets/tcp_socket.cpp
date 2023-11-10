#include "tcp_socket.h"
#include "vlog.h"
#include "ale_error_handler.h"
#include "vposix_core.h"

AleTcpSocket::AleTcpSocket( SOCKET_TYPE type, const uint32_t data_wait_period_ms )
        : _socket_type(type),
          _socket_status(IDLE),
          _data_wait_period(std::chrono::milliseconds(data_wait_period_ms))
{
    if (_socket_type == CLIENT)
    {
        _socket = std::make_shared<VTcpSocket>();

        tuneSocketSignals();
        tuneReadyRead();

        _socket->socket_connected.connect([this]()
        {
            vtrace << "ALE: Сокет подключен к адресу:" << this->_ip << "порт" << this->_port
                   << "num of disconnections =" << this->_num_of_disconnections;

            _socket_status = OK;

            // Установка опции KeepAlive
            _socket->set_keep_alive(1,1,10);

            _parser.clearBuf();
            socket_connected();
        });

        _data_wait_timer.timeout = [this]()
        {
            _data_wait_timer.stop();

            vrunlog << "ALE: Timeout" << _data_wait_period.count() << "ms of waiting data from" << _ip << "port" << _port << "Disconnect.";
            disconnect();
        };
    }
}

AleTcpSocket::AleTcpSocket(SOCKET_TYPE type, VTcpSocket::Peer& peer)
        : _socket_type(type)
{
    if ( _socket_type != WAIT_SERVER )
    {
        vfatal << "В сокет с типом" << _socket_type << "нельзя вставлять серверное подключение!";
        return;
    }

    _socket = std::make_shared<VTcpSocket>(&peer);

    _socket_status = OK;

    tuneSocketSignals();
    tuneReadyRead();
    tuneWaitServerSignals();
}

void AleTcpSocket::changeSocketType( const SOCKET_TYPE type )
{
    _socket_type = type;
}

void AleTcpSocket::tuneReadyRead()
{
    _parser.received_au1 .connect( [this](AlepktAu1    au1){received_au1(au1) ;});
    _parser.received_au2 .connect( [this](AlepktAu2    au2){received_au2(au2) ;});
    _parser.received_dt  .connect( [this](AlepktHeader dt) {received_dt (dt)  ;});
    _parser.received_di  .connect( [this](AlepktHeader di) {received_di (di)  ;});

    _socket->ready_read.connect([this]()
    {
        if (_socket->is_connected())
        {
            _data_wait_timer.stop();
            _data_wait_timer.start(_data_wait_period);

            vbyte_buffer dt(_socket->receive_all());
            _parser.appendData(dt);
        }
        else
        {
            vdeb << "Попытка чтения из отключенного сокета. Disconnecting..";
            disconnect();
        }
    });
}

void AleTcpSocket::tuneSocketSignals()
{
    if ( _socket.get() != nullptr )
    {
        _socket->socket_disconnected.connect( [this](){ socket_disconnected(); });
    }
}

void AleTcpSocket::tuneWaitServerSignals()
{
    received_au1.connect([this](AlepktAu1 au1)
    {
        if (_socket_type != SOCKET_TYPE::WAIT_SERVER)
        {
            vwarning << "ALE: В сокет типа" << _socket_type << "неожиданно пришёл пакет AU1";
            return;
        }

        vtrace << "ALE: В ожидающий сокет пришёл AU1";

        if (au1.qos != q_of_service)
        {
            AleErrorHandler::printAleError(1, 2);
            disconnect();

            wait_socket_fail();
        }
        else
        {
            wait_socket_received_AU1(au1);
        }
    });

    received_di.connect([this](AlepktHeader)
    {
        vdeb << "ALE: Подключение к серверу было разорвано до прихода ALEPKT AU1.";

        disconnect();
        wait_socket_fail();
    });
}

void AleTcpSocket::unlinkOwnSignals()
{
    received_au2.   disconnect_all();
    received_dt.    disconnect_all();
    received_di.    disconnect_all();
}

void AleTcpSocket::unlinkWaitServerSignals()
{
    wait_socket_fail.disconnect_all();
}

void AleTcpSocket::connect(vbyte_buffer ip, uint16_t port)
{
    vdeb << "ALE: Trying to connect to" << ip.data() << port;
    this->_ip = ip;
    this->_port = port;

    _socket_status = WAIT_FOR_CONNECT;

    try
    {
        _data_wait_timer.start(_data_wait_period);
        _socket->connect_to_host(VIpAddress(ip), port);
    }
    catch (...)
    {
        vfatal << "ALE: Неудачная попытка установления соединения";        
        disconnect();
    }
}

AleTcpSocket::SOCKET_STATUS AleTcpSocket::getSocketStatus()
{
    return _socket_status;
}

void AleTcpSocket::setSocketStatus(AleTcpSocket::SOCKET_STATUS status)
{
    _socket_status = status;
}

void AleTcpSocket::disconnect()
{
    _parser.clearBuf();
    _data_wait_timer.stop();

    if (_socket->is_connected())
    {
        vdeb << "Сокет отключился";
        _socket->disconnect_from_host();
    }
    else
        vfatal << "ALE: Попытка дисконнекта неподключенного сокета";

    _socket_status = IDLE;
    socket_disconnected();
}

void AleTcpSocket::send(vbyte_buffer alepkt)
{
    if (_socket->is_connected())
    {
        try {

            _socket->send(alepkt);

        }  catch (...) {

            vdeb << "ALE: Ошибка при отправке пакета. Disconnect...";
            disconnect();
            return;
        }

        succsessful_send();
    }
    else
        vfatal << "ALE: Попытка послать данные по неподключенному сокету";
}

const std::shared_ptr<VTcpSocket> AleTcpSocket::getSocket() const
{
    return _socket;
}
