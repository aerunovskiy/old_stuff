#include "ale.h"
#include "socket_pair.h"
#include "tcp_socket.h"
#include "ale_error_handler.h"

#include "vbyte_buffer_view.h"
#include "vapplication.h"
#include "vlog.h"

#include <unistd.h>

void SocketPair::initSocketPair( int pair_id )
{
    this->pair_id = pair_id;
    if ( pair_id == Ale::max_num_of_server_connections )
    {
        _pair_type = CLIENT;

        _socket[AleTcpSocket::NR_FLAG::N] = std::make_shared<AleTcpSocket>(AleTcpSocket::SOCKET_TYPE::CLIENT, _socket_wait_period_ms);
        socketInit(AleTcpSocket::NR_FLAG::N);

        _socket[AleTcpSocket::NR_FLAG::R] = std::make_shared<AleTcpSocket>(AleTcpSocket::SOCKET_TYPE::CLIENT, _socket_wait_period_ms);
        socketInit(AleTcpSocket::NR_FLAG::R);
    }
    else if ( (pair_id < Ale::max_num_of_server_connections) && (pair_id >= 0) )
    {
        _pair_type = SERVER;
    }
    else
    {
        vfatal << "Неверный номер пары сокетов при инициализации:" << pair_id;
        _pair_type = UNDEFINED;
    }
}

void SocketPair::setSocketWaitPeriod(const uint32_t socket_wait_period)
{
    _socket_wait_period_ms = socket_wait_period;
}

void SocketPair::socketInit(AleTcpSocket::NR_FLAG nr)
{
    tsn[AleTcpSocket::NR_FLAG::N] = tsn[AleTcpSocket::NR_FLAG::R] = 0;
    // Отвязываем сигналы сокета от привязанных к ним методам
    _reconnect_timer[nr].timeout.unlink();

    _reconnect_timer[nr].timeout = [this, nr]()
    {
        if ( _socket[nr] != nullptr &&
             _socket[nr]->getSocket() != nullptr &&
             !_socket[nr]->getSocket()->is_connected() &&
             _socket[nr]->getSocketStatus() != AleTcpSocket::SOCKET_STATUS::WAIT_FOR_CONNECT )
        {
            vdeb << "Попытка переподключения клиентского сокета (" << nr << ")...";
            _socket[nr]->connect(_network_address[nr].address.str(), _network_address[nr].port);
        }
        else
        {
            _reconnect_timer[nr].stop();
        }
    };

    _socket[nr]->socket_connected.connect([this, nr]()
    {
        vtrace << "До: TSN["<<(int)nr<<"]="<<tsn[nr]<<";TSN["<<(int)(!nr)<<"]="<<tsn[!nr];

        if (this->_socket[!nr].get()->getSocket()->is_connected() && _status == OK)
            tsn[nr] = tsn[!nr];

        vtrace << "После: TSN["<<(int)nr<<"]="<<tsn[nr]<<";TSN["<<(int)(!nr)<<"]="<<tsn[!nr];
        _reconnect_timer[nr].stop();

        if (_pair_type == CLIENT)
        {
            client_sock_connected(nr);
        }
    });

    _socket[nr]->socket_disconnected.connect([this, nr]()
    {
        this->_socket[nr].get()->increaseNumOfDisconnections();

        // Смотрим, серверный или клиентский сокет
        std::string type = _pair_type == SERVER ? "Серверный" : CLIENT ? "Клиентский" : "";

        if (nr == AleTcpSocket::NR_FLAG::N)
            vdeb << "Socket Pair:" << type << "сокет (Normal) "
                 << "отключился TCEPID =" << pair_id;

        else if (nr == AleTcpSocket::NR_FLAG::R)
            vdeb << "Socket Pair:" << type << "сокет (Redundant) "
                 << "отключился TCEPID =" << pair_id;

        if ( this->isVacant() )
        {
            // If another socket is not exists or not waiting for TCP or disconnected
            if ( getTCPSocket(static_cast<AleTcpSocket::NR_FLAG>(!nr)) == nullptr ||
                 getTCPSocket(static_cast<AleTcpSocket::NR_FLAG>(!nr))->getSocketStatus() == AleTcpSocket::IDLE )
            {
                setStatus(DISC);
            }

            if ( getStatus() == DISC )
            {
                vdeb << "ALE: The last socket in a pair. ETCSID =" << peer_etcsid;
                _reconnect_timer[nr].stop();
                _reconnect_timer[!nr].stop();
                sock_disc(static_cast<uint16_t>(pair_id));
            }
            else
            {
                if ( _pair_type == CLIENT )
                {
                    vdeb << "ALE: Attempt to reconnect disconnected socket. ETCSID =" << peer_etcsid;
                    _reconnect_timer[nr].start(std::chrono::seconds(2));
                }
            }
        }
        else
        {
            if ( _pair_type == CLIENT && getStatus() != DISC )
            {
                vdeb << "ALE: Attempt to reconnect disconnected socket. ETCSID =" << peer_etcsid;
                _reconnect_timer[nr].start(std::chrono::seconds(2));
            }
        }

//        if (this->isVacant() && _status != DISC)
//        {
//            vdeb << "Последний сокет в паре";
//            _reconnect_timer.stop();
//            _status = DISC;
//            sock_disc(static_cast<uint16_t>(pair_id));
//        }
//        else if (_pair_type == CLIENT && _status != DISC)
//        {
//            vdeb << "Попытка переподключения...";
//            _reconnect_timer.start(std::chrono::seconds(5));
//        }
    });

    // В Socket_Pair не может прийти AU1
    _socket[nr]->received_au1.connect([nr](AlepktAu1)
    {
        vwarning << "ALE: В связанный сокет" << static_cast<uint16_t>(nr) << "неожиданно пришел ALEPKT AU1."
                 << "Пакет отклоняется";
    });

    _socket[nr]->received_au2.connect([this, nr](AlepktAu2 au2)
    {
        std::string nrflag = (nr == AleTcpSocket::NR_FLAG::N) ? "Normal" : "Redundant";

        if (_status == WAIT_AU2 && _pair_type == CLIENT)
        {
            // Если Application Type пакета не поддерживается, соединение разрывается
            if (au2.application_type != app_type)
            {
                sock_disc(pair_id);
                //this->clearPair();
                AleErrorHandler::printAleError(6, 1);
            }
            else
            {
                _status = OK;
                received_au2(au2, static_cast<uint16_t>(pair_id));
            }
        }
        else if (_status == OK)
        {
            vwarning << "ALE: Связанный сокет(" << nrflag << ")получил ALEPKT AU2, однако соединение уже установлено.";
        }
        else
        {
            vwarning << "ALE: Связанный сокет(" << nrflag << ")неожиданно получил ALEPKT AU2." << "Пакет отклоняется";
        }
    });

    _socket[nr]->received_dt.connect([this, nr](AlepktHeader dt)
    {
        if (_status == OK)
        {
            // Если Application Type пакета не поддерживается, соединение разрывается
            if (dt.application_type != app_type)
            {
                sock_disc(pair_id);
                AleErrorHandler::printAleError(6, 1);
            }
            else
                // Проверка на пересечение TSN максимального значения
            if (dt.tsn > _last_tsn)
            {
                if (dt.tsn == max_tsn)
                    _last_tsn = -1;
                else
                    _last_tsn = dt.tsn;

                received_dt(dt, static_cast<uint16_t>(pair_id), static_cast<bool>(nr));
                vtrace << "SocketPair: CHANNEL =" << static_cast<bool>(nr);
            }
        }
        else
        {
            vwarning << "ALE: Связанный сокет неожиданно получил ALEPKT DT. Пакет отклоняется";
        }
    });

    _socket[nr]->received_di.connect([this](AlepktHeader di)
    {
        received_di(di, static_cast<uint16_t>(pair_id));
        _status = DISC;
    });

    if (nr == AleTcpSocket::N)
        _socket[nr]->succsessful_send.connect(this, &SocketPair::onNormalSuccessfulSend);
    else
        _socket[nr]->succsessful_send.connect(this, &SocketPair::onRedundantSuccessfulSend);
}

void SocketPair::inputSock( AleTcpSocket::NR_FLAG nr,
                            std::shared_ptr<AleTcpSocket> sock,
                            uint32_t peer_etcsid                )
{
    // Если пустая пара, присваиваем ETCSID и вставляем сокет
    if (this->peer_etcsid == -1)
    {
        vtrace << "Вставляем сокет в пустую пару...";
        this->peer_etcsid = static_cast<int>(peer_etcsid);

        _socket[nr] = std::move(sock);
        sock.reset();

        if (_pair_type == SERVER)
        {
            // Меняем типа сокета на серверный
            _socket[nr]->changeSocketType(AleTcpSocket::SOCKET_TYPE::SERVER);

            // Отвязываем "ждущие" сигналы
            _socket[nr]->unlinkWaitServerSignals();

            // Отвязываем сигналы сокета, потому что они были связаны со "ждушими"
            _socket[nr]->unlinkOwnSignals();

            // Привязываем сигналы к логике работы Socket Pair
            socketInit(nr);
        }
    }
    else
    {
        // Иначе проверяем на соответствие
        if (this->peer_etcsid != static_cast<int>(peer_etcsid))
            vfatal << "ALE: Попытка вставить сокет в пару"
                   << "с несоответствующим peer_ETCSID";
        else
            // Если тот ETCSID, проверяем, не занят ли сокет
        if (!isSockVacant(nr))
            vfatal << "ALE: Попытка вставить сокет в занятую пару";
        else
        {
            _socket[nr] = std::move(sock);
            sock.reset();

            if (_pair_type == SERVER)
            {
                // Меняем типа сокета на серверный
                _socket[nr]->changeSocketType(AleTcpSocket::SOCKET_TYPE::SERVER);

                // Отвязываем "ждущие" сигналы
                _socket[nr]->unlinkWaitServerSignals();

                // Отвязываем сигналы сокета, потому что они были связаны со "ждушими"
                _socket[nr]->unlinkOwnSignals();

                // Привязываем сигналы к логике работы Socket Pair
                socketInit(nr);
            }

            // Присваиваем TSN
            tsn[nr] = tsn[!nr];
        }
    }
}

void SocketPair::parseSingleAddress(    std::string::const_iterator&  p,
                                        std::string::const_iterator&  end_it,
                                        vbyte_buffer&                 ip,
                                        uint16_t&                     port    )
{
    if (p == end_it || !(*p)) return;

    if (*p == '0')
    {
        ip.clear();
        port = 0;
        return;
    }

    while (p != end_it && *p != ':') ip.append(*(p++));

    vbyte_buffer temp;

    while (p != end_it && *(p++) != ';') temp.append(*p);

    port = temp.to_u16();
}

void SocketPair::initNetworkAddress( const std::string&  network_address )
{
    vbyte_buffer       ip1;
    vbyte_buffer       ip2;

    uint16_t           port1;
    uint16_t           port2;

    auto p =   network_address.begin();
    auto end = network_address.end();

    parseSingleAddress(p, end, ip1, port1);

    _network_address[AleTcpSocket::NR_FLAG::N].address = VIpAddress(ip1);
    _network_address[AleTcpSocket::NR_FLAG::N].port = port1;

    parseSingleAddress(p, end, ip2, port2);

    _network_address[AleTcpSocket::NR_FLAG::R].address = VIpAddress(ip2);
    _network_address[AleTcpSocket::NR_FLAG::R].port = port2;

}

void SocketPair::parseNetworkAddress(   const std::string&  network_address,
                                        vbyte_buffer&       ip1,
                                        vbyte_buffer&       ip2,
                                        uint16_t&           port1,
                                        uint16_t&           port2               )
{
    auto p   = network_address.begin();
    auto end = network_address.end();

    parseSingleAddress(p, end, ip1, port1);

    parseSingleAddress(p, end, ip2, port2);
}

const std::shared_ptr<VTcpSocket> SocketPair::getSocket(AleTcpSocket::NR_FLAG nr) const
{
    if (_socket[nr] != nullptr)
        return _socket[nr]->getSocket();
    else
        return nullptr;
}

const std::shared_ptr<AleTcpSocket> SocketPair::getTCPSocket(AleTcpSocket::NR_FLAG nr) const
{
    if (_socket[nr])
        return _socket[nr];
    else
        return nullptr;
}


void SocketPair::connect(const std::string& network_address)
{
    vbyte_buffer        ip1, ip2;
    uint16_t            port1 = 0, port2 = 0;

    parseNetworkAddress(network_address, ip1, ip2, port1, port2);

    // Смотрим, свободен ли Socket_Pair
    // Если свободен, то подключаемся
    if (_status == DISC)
    {
        setStatus(WAIT_TCP);

        if (this->_socket[AleTcpSocket::NR_FLAG::N].get() && port1 != 0 )
            this->_socket[AleTcpSocket::NR_FLAG::N]->setSocketStatus(AleTcpSocket::SOCKET_STATUS::WAIT_FOR_CONNECT);

        if (this->_socket[AleTcpSocket::NR_FLAG::R].get() && port2 != 0 )
            this->_socket[AleTcpSocket::NR_FLAG::R]->setSocketStatus(AleTcpSocket::SOCKET_STATUS::WAIT_FOR_CONNECT);

        if (this->_socket[AleTcpSocket::NR_FLAG::N].get() && port1 != 0 )
            this->_socket[AleTcpSocket::NR_FLAG::N]->connect(ip1, port1);

        if (this->_socket[AleTcpSocket::NR_FLAG::R].get() && port2 != 0 )
            this->_socket[AleTcpSocket::NR_FLAG::R]->connect(ip2, port2);

    }
    else
        vfatal << "ALE: Попытка подключения в уже подключенной паре";
}

void SocketPair::clearPair()
{
    tsn[0] = 0;
    tsn[1] = 0;

    // Очищаем все параметры
    peer_etcsid = -1;
    _last_tsn    = -1;

    _status = DISC;

    _reconnect_timer[AleTcpSocket::NR_FLAG::N].stop();
    _reconnect_timer[AleTcpSocket::NR_FLAG::R].stop();

    if ( _pair_type == SERVER )
    {
        _reconnect_timer[AleTcpSocket::NR_FLAG::N].timeout.unlink();
        _reconnect_timer[AleTcpSocket::NR_FLAG::R].timeout.unlink();
    }

    //  Очищаем клиентские сокеты
    if ( _pair_type == CLIENT )
    {

        for (auto &s : _socket)
        {
            if ( s.get() && s->getSocket() != nullptr && s->getSocket()->is_connected())
            {
                s->disconnect();
            }
        }

       // client_sock_connected.disconnect_all();
    }

    pair_cleared();
}

bool SocketPair::isVacant()
{
    // Проверяем, есть ли там вообще сокеты
    bool w = _socket[AleTcpSocket::NR_FLAG::N].get();
    bool v = _socket[AleTcpSocket::NR_FLAG::R].get();

    // Если есть, смотрим, законекчены они или нет
    w = w ? (_socket[AleTcpSocket::NR_FLAG::N].get()->getSocket()!=nullptr) : w;
    v = v ? (_socket[AleTcpSocket::NR_FLAG::R].get()->getSocket()!=nullptr) : v;

    w = w ? _socket[AleTcpSocket::NR_FLAG::N].get()->getSocket()->is_connected() : w;
    v = v ? _socket[AleTcpSocket::NR_FLAG::R].get()->getSocket()->is_connected() : v;

    return (!( w || v ));
}

bool SocketPair::isSockVacant(AleTcpSocket::NR_FLAG fl)
{
    // Проверяем, есть ли там вообще сокеты
    bool w = _socket[fl].get();

    // Если есть, смотрим, законекчены они или нет
    w = w ? _socket[fl].get()->getSocket()->is_connected() : w;

    return (! w );
}

bool SocketPair::isSockExists(AleTcpSocket::NR_FLAG fl)
{
    // Проверяем, есть ли там вообще сокеты
    return _socket[fl].get();
}

void SocketPair::send(AleTcpSocket::NR_FLAG fl, vbyte_buffer alepkt)
{
    // Смотрим, есть ли в данной паре нужный TCEPID
    // Если есть, то отправляем по соответствующему сокету данные
    if (
            this->_socket[fl].get() &&
                    (this->_socket[fl].get()->getSocket() != nullptr) &&
                    (this->_socket[fl].get()->getSocket().get() != nullptr) &&
                     this->_socket[fl].get()->getSocket()->is_connected()
            )
        this->_socket[fl]->send(alepkt);
    else
    {
        vwarning << "Попытка послать данные в паре с TCEPID = " << this->pair_id << "по неподключенному сокету";

        // Если второго нет, то дисконнект
        if ( this->isSockVacant(static_cast<AleTcpSocket::NR_FLAG>(!fl)) && _status != DISC)
        {
            vdeb << "Последний сокет в паре";
            _reconnect_timer[fl].stop();
            _reconnect_timer[!fl].stop();
            _status = DISC;
            sock_disc(static_cast<uint16_t>(pair_id));
        }
    }
}

void SocketPair::setStatus(STATUS st)
{
    _status = st;
}

const SocketPair::STATUS SocketPair::getStatus()
{
    return _status;
}

void SocketPair::onNormalSuccessfulSend()
{
    active_channel(true);
}

void SocketPair::onRedundantSuccessfulSend()
{
    active_channel(false);
}
