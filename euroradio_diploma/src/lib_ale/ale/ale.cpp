#include "vtimer.h"
#include "vapplication.h"

#include "ale.h"
#include "alepkt_builder.h"
#include "ale_error_handler.h"
#include "vsettings.h"

using namespace std;

// Счётчик для ID WaitUnit
uint16_t Ale::WaitUnit::counter = 0;

Ale::Ale(const vsettings &settings)
{
    auto _settings = settings.subgroup("ALE");

    etcsid = _settings.subgroup("ETCSID").get<uint32_t>("etcsid");

    auto all_etcsid = _settings.subgroup("AddressBook").keys();

    for ( const auto &etcsid: all_etcsid)
    {
        etcsid_address_book[stoi(etcsid)] = _settings.subgroup("AddressBook").get(etcsid);
        if (!_settings.subgroup("TypeBook").has_key(etcsid))
        {
            vfatal << "Тип объекта с ETCSID" << etcsid << "отсутствует в ini-файле.";
            VApplication::app()->stop();
        }
        etcsid_type_book[stoi(etcsid)] = _settings.subgroup("TypeBook").get<int>(etcsid);
    }

    // Устанавливаем таймер
    t_wait = _settings.subgroup("Timer").get<int>("wait_response");

    _socket_wait_period_ms = _settings.subgroup("Timer").get<uint32_t>("socket_data_wait_period_ms");

    // Инициализируем все пары
    initPairs();

    // Заводим сборщик мусора
    initGarbageCollector();

    // Заводим сервер
    waitForConnect();

    _client_pair.active_channel.connect(this, &Ale::onChannelActive);
}

void Ale::initPairs()
{
    // Инициализируем 100 серверных пар
    for (int i = 0; i < max_num_of_server_connections; ++i)
    {
        // Каждой паре присваиваем уникальный TCEPID
        _server_pair[i].initSocketPair(i);

        _server_pair[i].sock_disc.connect(this, &Ale::onSockDisconnected);

        // Изначальный статус каждой пары - соединение отсутствует
        _server_pair[i].setStatus(SocketPair::STATUS::DISC);

        // Серверная пара не может запрашивать соединение
        _server_pair[i].received_au2.connect([](AlepktAu2, uint16_t)
        {
            vfatal << "ALE: Серверной паре пришел ALEPKT AU2. "
                   << "Серверная пара не имеет права запрашивать соединение.";
        });

        _server_pair[i].received_dt.connect(this, &Ale::onDtReceived);
        _server_pair[i].received_di.connect(this, &Ale::onDiReceived);
    }

    // 101-я пара - клиентская
    _client_pair.setSocketWaitPeriod(_socket_wait_period_ms);
    _client_pair.initSocketPair(max_num_of_server_connections);
    _client_pair.setStatus(SocketPair::STATUS::DISC);
    _client_pair.sock_disc.connect(this, &Ale::onSockDisconnected);

    _client_pair.received_au2.  connect (this, &Ale::onAu2Received);
    _client_pair.received_dt.   connect (this, &Ale::onDtReceived );
    _client_pair.received_di.   connect (this, &Ale::onDiReceived );
}

void Ale::initGarbageCollector()
{
    _garbage_buf_collector.timeout = [this]()
    {
        if (_sock_buf.empty()) return;

        auto it = _sock_buf.begin();
        while (it != _sock_buf.end())
        {
            if ( it->get() != nullptr && it->get()->getStatus()==WaitUnit::WAIT_UNIT_STATUS::OUTDATED )
            {
                it = _sock_buf.erase(it);
            }
            else
            {
                ++it;
            }
        }
    };

    // WARNING! MAY CRUSH THE SYSTEM - NEED TESTS
    _garbage_buf_collector.start(std::chrono::seconds(1));
}

void Ale::waitForConnect()
{
    // Получаем IP-адрес и порт данной сущности
    vbyte_buffer ip1, ip2;
    uint16_t port1, port2;
    SocketPair::parseNetworkAddress(etcsid_address_book[etcsid], ip1, ip2, port1, port2);

    if ( !ip1.empty() && port1 != 0 )
        _server[0].listen(VIpAddress(ip1), port1);

    if ( !ip2.empty() && port2 != 0 )
        _server[1].listen(VIpAddress(ip2), port2);

    _server[0].peer_connected.connect(this, &Ale::onPeerConnected);
    _server[1].peer_connected.connect(this, &Ale::onPeerConnected);
}

void Ale::onPeerConnected( VTcpSocket::Peer peer)
{
    vdeb << "Установлено подключение к серверу. Создан серверный сокет";

    // Добавляем сокет в чистилище
    _sock_buf.push_back( std::make_shared<WaitUnit> (   std::make_shared<AleTcpSocket>(AleTcpSocket::WAIT_SERVER, peer),
                                                        Ale::t_wait                                                         ) );

    // Настраиваем сокет
    tuneSocket(_sock_buf.back().get());
}

void Ale::onAu1Received(AlepktAu1 au1, std::shared_ptr<AleTcpSocket> sock )
{
    vdeb << "ALE: Получен пакет ALEPKT AU1";
    int find = -1;
    bool is_first = false;

    // При отсутствии вызывающего ETCSID в таблице ETCSID соединение разрывается
    if (etcsid_address_book[au1.calling_etcsid].empty())
    {
        vfatal << "Неизвестный ETCSID=" << au1.calling_etcsid << ". Соединение разрывается.";
        sock->disconnect();

        AleErrorHandler::printAleError(1, 1);
        return;
    }

    // Ищем пару с соответствующим peer_ETCSID
    for (int j = 0; j < max_num_of_server_connections; ++j)
        if (_server_pair[j].peer_etcsid ==
                static_cast<int>(au1.calling_etcsid))
        {
            vtrace << "Пара с ETCSID=" << au1.calling_etcsid << "уже существует(" << j << ");";
            find = j;
            break;
        }

    // Если не нашли, ищем первую незанятую пару
    if (find == -1)
        for (int j = 0; j < max_num_of_server_connections; ++j)
            if (_server_pair[j].isVacant())
            {
                is_first    = true;
                find        = j;
                break;
            }

    // Если все заняты, выводим ошибку и разрываем соединение
    if (find == -1)
    {
        vdeb << "Нет свободных пар. Закрываем соединение...";
        sock->disconnect();

        AleErrorHandler::printAleError(2, 1);

        return;
    }

    // Вставляем сокет в нужную пару
    _server_pair[find].inputSock(   static_cast<AleTcpSocket::NR_FLAG>(au1.nr),
            sock,
            au1.calling_etcsid);
    sock.reset();

    if (au1.nr == AleTcpSocket::NR_FLAG::N)
        vdeb << "ALE: Серверный сокет с TCEPID" << find
             << "подключен (Normal Channel)";
    else
    if (au1.nr == AleTcpSocket::NR_FLAG::R)
        vdeb << "ALE: Серверный сокет с TCEPID" << find
             << "подключен (Redundant Channel).";

    if (is_first)
    {
        // Заполняем необходимые данные для индикации
        ConnectIndicationData dt;

        dt.q_of_s           = au1.qos;
        dt.tcepida          = static_cast<uint16_t>(find);
        dt.calling_etcsid   = au1.calling_etcsid;
        dt.network_address  = etcsid_address_book[au1.calling_etcsid];
        dt.application_type = au1.application_type;
        dt.user_data        = au1.user_data;

        // Сообщаем о получении AU1
        connect_indication(dt);
    }
}

void Ale::onAu2Received(AlepktAu2 au2, uint16_t tcepid)
{
    vdeb << "ALE: Получен пакет ALEPKT AU2";
    ConnectConfirmationData con_conf;

    con_conf.tcepidb            = tcepid;
    con_conf.user_data          = au2.user_data;
    con_conf.responding_etcsid  = au2.responding_etcsid;

    _client_pair.setStatus(SocketPair::STATUS::OK);

    connect_confirmation(con_conf);
}

void Ale::onDtReceived (AlepktHeader dt, uint16_t tcepid, bool channel)
{
    vdeb << "ALE: Получен пакет ALEPKT DT";
    DataIndicationData data;

    data.user_data  = dt.user_data;
    data.tcepidrcv  = tcepid;
    data.channel    = channel;

    data_indication(data);
}

bool Ale::isServerPair(uint16_t tcepid)
{
    return (tcepid < max_num_of_server_connections);
}

bool Ale::isClientPair(uint16_t tcepid)
{
    return (tcepid == max_num_of_server_connections);
}

SocketPair* Ale::getPair(uint16_t tcepid)
{
    if (isServerPair(tcepid))
    {
        if (!_server_pair[tcepid].isVacant())
            return &_server_pair[tcepid];
        else
        {
            vwarning << "Попытка получить неподключенную пару сокетов";
            return &_server_pair[tcepid];
        }
    }
    else if (isClientPair(tcepid))
    {
        if (!_client_pair.isVacant())
            return &_client_pair;
        else
        {
            vwarning << "Попытка получить неподключенную пару сокетов. TCEPID=" << tcepid;
            return &_client_pair;
        }
    }
    else
    {
        vwarning << "Попытка получить пару сокетов с невалидным TCEPID";
        return nullptr;
    }
}

void Ale::onSockDisconnected (uint16_t tcepid)
{
    vtrace << "ALE: socket_disconnected recieved ";

    getPair(tcepid)->pair_cleared.connect([this, tcepid]()
    {
        DisconnectIndicationData dt(tcepid, 1, 3);
        vtrace << "ALE: call disconnect_indication";
        disconnect_indication(dt);
    });

    getPair(tcepid)->clearPair();
    getPair(tcepid)->pair_cleared.disconnect_all();
}

void Ale::onDiReceived (AlepktHeader di, uint16_t tcepid)
{    
    vbyte_buffer data = di.user_data;

    getPair(tcepid)->pair_cleared.connect([this, tcepid, data]()
    {
        vdeb << "ALE: Пары очищены, посылка сигнала Disconnect Indication..";
        DisconnectIndicationData disc(tcepid, 0, 0, data);
        disconnect_indication(disc);
    });

    vdeb << "ALE: Получен пакет ALEPKT DI. Очищаю пары...";

    getPair(tcepid)->clearPair();

    getPair(tcepid)->pair_cleared.disconnect_all();
}

void Ale::tuneSocket( WaitUnit* wait_unit )
{
    wait_unit->wait_unit_received_au1.connect( [this, wait_unit](uint16_t id, AlepktAu1 au1)
    {
        onAu1Received(au1, wait_unit->getTcpSocket());
        _sock_buf[id]->setStatus(WaitUnit::WAIT_UNIT_STATUS::OUTDATED);
    });

    wait_unit->wait_unit_error.connect([this](uint16_t id)
    {
        _sock_buf[id]->setStatus(WaitUnit::WAIT_UNIT_STATUS::OUTDATED);
    });

    wait_unit->startTimer();
}

void Ale::connectRequest(   uint32_t      address_type,
                            const string& network_address,
                            uint32_t      called_etcsid,
                            uint32_t      calling_etcsid,
                            uint8_t       application_type,
                            uint8_t       qos,
                            const string& user_data
                        )
{
    // Проверяем, свободны ли клиент сокеты
    if (!_client_pair.isVacant())
    {
        AleErrorHandler::printAleError(2, 1);
        return;
    }

    // Получаем (ETCSID + ETY)
    called_etcsid  |= static_cast<uint32_t>(etcsid_type_book[called_etcsid]  << 24);
    calling_etcsid |= static_cast<uint32_t>(etcsid_type_book[calling_etcsid] << 24);

    _client_pair.peer_etcsid = called_etcsid;

    _client_pair.client_sock_connected.disconnect_all();
    // Соединяем сокеты
    _client_pair.connect(network_address);

    _client_pair.initNetworkAddress(network_address);

    // Формируем ALEPKT AU1
    AlepktAu1 au1;

    au1.nr              = AleTcpSocket::NR_FLAG::N;
    au1.user_data       = vbyte_buffer(user_data);
    au1.calling_etcsid  = calling_etcsid;
    au1.called_etcsid   = called_etcsid;

    _client_pair.client_sock_connected.connect([this, au1](AleTcpSocket::NR_FLAG nr)
    {
        vdeb << "Socket connected";

        if (_client_pair.getStatus() != SocketPair::STATUS::OK)
        {
            AlepktAu1 au1_nr(au1);

            au1_nr.nr = nr;
            std::string channel = nr ? "Normal" : "Redundant";

            vtrace << "ALE: Клиентский сокет (" << channel << "Channel) подключен."
                   << "Посылка ALEPKT AU1...";

            // Шлем ALEPKT AU1
            _client_pair.send(nr,
                             AlepktBuilder::formAlepktAu1(au1)   );

            _client_pair.setStatus(SocketPair::STATUS::WAIT_AU2);

            _client_pair.tsn[au1.nr]++;
            report_tsn(max_num_of_server_connections, _client_pair.tsn[au1.nr]);
        }
    });
}

void Ale::connectResponse     ( uint16_t            tcepida,
                                uint32_t            responding_etcsid,
                                const std::string&  user_data            )
{
    // Формируем ALEPKT AU2
    AlepktAu2 au2;

    // Получаем (ETCSID + ETY)
    responding_etcsid |= static_cast<uint32_t>(etcsid_type_book[responding_etcsid] << 24);

    au2.responding_etcsid   = responding_etcsid;
    au2.user_data           = vbyte_buffer(user_data);
    au2.nr                  = AleTcpSocket::NR_FLAG::N;

    vdeb << "ALE: Посылка ALEPKT AU2...";

    if (    _server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::N) != nullptr &&
            _server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::N)->getSocket() &&
            _server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::N)->getSocket()->is_connected() )
    {
        vdeb << "ALE: Канал N найден";
        _server_pair[tcepida].send(AleTcpSocket::NR_FLAG::N,
                              AlepktBuilder::formAlepktAu2(au2) );
        vdeb << "ALE: Канал N послал AU2 SaPDU";
        _server_pair[tcepida].tsn[au2.nr]++;
        report_tsn(tcepida, _server_pair[tcepida].tsn[au2.nr]);
    }
    else
        vdeb << "ALE: Канал N не найден";

    if (_server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::R) != nullptr &&
            _server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::R)->getSocket() &&
            _server_pair[tcepida].getTCPSocket(AleTcpSocket::NR_FLAG::R)->getSocket()->is_connected())
    {
        vdeb << "ALE: Канал R найден";
        au2.nr = AleTcpSocket::NR_FLAG::R;
        _server_pair[tcepida].send(AleTcpSocket::NR_FLAG::R,
                                  AlepktBuilder::formAlepktAu2(au2) );
        vdeb << "ALE: Канал R послал AU2 SaPDU";
        _server_pair[tcepida].tsn[au2.nr]++;
        report_tsn(tcepida, _server_pair[tcepida].tsn[au2.nr]);
    }
    else
        vdeb << "ALE: Канал R не найден";

    _server_pair[tcepida].setStatus(SocketPair::STATUS::OK);
}

void Ale::disconnectRequest   ( uint16_t            tcepidxmt,
                                const std::string&  user_data     )
{
    SocketPair* pair = getPair(tcepidxmt);
    if (pair == nullptr)
    {
        return;
    }

    if (pair->getStatus() == SocketPair::DISC)
    {
        return;
    }
    // Формируем ALEPKT DI
    AlepktHeader head;

    head.tsn        = 0;
    head.packet_type = PACKET_TYPE::DI;
    head.user_data   = vbyte_buffer(user_data);

    vdeb << "ALE: TCEPID" << tcepidxmt << "Посылка ALEPKT DI...";

    if (    pair->getTCPSocket(AleTcpSocket::NR_FLAG::N) != nullptr &&
            pair->getTCPSocket(AleTcpSocket::NR_FLAG::N)->getSocket()->is_connected() )
    {
        head.nr = AleTcpSocket::NR_FLAG::N;
        vdeb << "ALE: Канал Normal послал ALEPKT DI... TCEPID ="
             << tcepidxmt;

        pair->send(AleTcpSocket::NR_FLAG::N,
                                    AlepktBuilder::formAlepktDtDi(head) );
    }
    else
        vdeb << "ALE: Канал Normal не найден. TCEPID =" << tcepidxmt;

    if (pair->getTCPSocket(AleTcpSocket::NR_FLAG::R) != nullptr &&
            pair->getTCPSocket(AleTcpSocket::NR_FLAG::R)->getSocket()->is_connected() )
    {
        head.nr = AleTcpSocket::NR_FLAG::R;
        vdeb << "ALE: Канал Redundant послал ALEPKT DI... TCEPID ="
             << tcepidxmt;
        pair->send(AleTcpSocket::NR_FLAG::R,
                                    AlepktBuilder::formAlepktDtDi(head) );
    }
    else
        vdeb << "ALE: Канал Redundant не найден. TCEPID =" << tcepidxmt;

    vdeb << "ALE: Разрываем соединение в паре TCEPID =" << tcepidxmt;

    pair->pair_cleared.connect([this, tcepidxmt]()
    {
        DisconnectIndicationData dt(tcepidxmt, 0, 0);
        disconnect_indication(dt);
    });

    // Разрываем связанные соединения
    pair->clearPair();
    pair->pair_cleared.disconnect_all();
}

void Ale::dataRequest(  uint16_t            tcepidxmt,
                        const std::string&  user_data     )
{
    // Формируем ALEPKT DT
    AlepktHeader head;

    head.packet_type = PACKET_TYPE::DT;
    head.user_data   = vbyte_buffer(user_data);

    vdeb << "ALE: TCEPID" << tcepidxmt << "Посылка ALEPKT DT...";

    if (getPair(tcepidxmt)->getTCPSocket(AleTcpSocket::NR_FLAG::N) != nullptr &&
            getPair(tcepidxmt)->getTCPSocket(AleTcpSocket::NR_FLAG::N)->getSocket()->is_connected())
    {
        head.nr     = AleTcpSocket::NR_FLAG::N;
        head.tsn    = getPair(tcepidxmt)->tsn[head.nr];

        getPair(tcepidxmt)->send(AleTcpSocket::NR_FLAG::N,
                                        AlepktBuilder::formAlepktDtDi(head) );
//        if (tcepidxmt < 100)
//            vdeb << "ALE: Канал Normal(server) отправил ALEPKT DT. TCEPID ="
//                 << tcepidxmt;
//        else if (tcepidxmt == 100)
//            vdeb << "ALE: Канал Normal(client) отправил ALEPKT DT. TCEPID ="
//                 << tcepidxmt;
    }
    else
    {
        if (tcepidxmt < 100)
            vdeb << "ALE: Канал Normal(server) не найден.";
        else if (tcepidxmt == 100)
            vdeb << "ALE: Канал Normal(client) не найден.";
    }

    // Проверка на превышение TSN максимума
    if (getPair(tcepidxmt)->tsn[head.nr] == SocketPair::max_tsn)
        getPair(tcepidxmt)->tsn[head.nr] = 0;
    else
        // Увеличиваем соответствующий TSN
        getPair(tcepidxmt)->tsn[head.nr]++;

    report_tsn(tcepidxmt, getPair(tcepidxmt)->tsn[head.nr]);

    if (    getPair(tcepidxmt)->getTCPSocket(AleTcpSocket::NR_FLAG::R) != nullptr &&
            getPair(tcepidxmt)->getTCPSocket(AleTcpSocket::NR_FLAG::R)->getSocket()->is_connected() )
    {
        head.nr     = AleTcpSocket::NR_FLAG::R;
        head.tsn    = getPair(tcepidxmt)->tsn[head.nr];

        //vdeb << "send TSN=" << getPair(TCEPIDXMT)->TSN[head.NR];

        getPair(tcepidxmt)->send(AleTcpSocket::NR_FLAG::R,
                                        AlepktBuilder::formAlepktDtDi(head) );
//        if (tcepidxmt < 100)
//            vdeb << "ALE: Канал Redundant(server) отправил ALEPKT DT. TCEPID ="
//                 << tcepidxmt;
//        else if (tcepidxmt == 100)
//            vdeb << "ALE: Канал Redundant(client) отправил ALEPKT DT. TCEPID ="
//                 << tcepidxmt;
    }
    else
    {
        if (tcepidxmt < 100)
            vdeb << "ALE: Канал Redundant(server) не найден.";
        else if (tcepidxmt == 100)
            vdeb << "ALE: Канал Redundant(client) не найден.";
    }

    if (getPair(tcepidxmt)->tsn[head.nr] == SocketPair::max_tsn)
        getPair(tcepidxmt)->tsn[head.nr] = 0;
    else
        // Увеличиваем соответствующий TSN
        getPair(tcepidxmt)->tsn[head.nr]++;

    report_tsn(tcepidxmt, getPair(tcepidxmt)->tsn[head.nr]);
}

const std::string& Ale::getNetworkAddress( uint32_t responding_etcsid )
{
    return etcsid_address_book[responding_etcsid];
}

void Ale::setTsn(uint16_t tcepid, uint16_t tsn)
{
    getPair(tcepid)->tsn[AleTcpSocket::NR_FLAG::N] = tsn;
    getPair(tcepid)->tsn[AleTcpSocket::NR_FLAG::R] = tsn;
}

Ale::~Ale()
{
    _garbage_buf_collector.stop();
}

void Ale::onChannelActive(bool channel)
{
    active_channel(channel);
}
