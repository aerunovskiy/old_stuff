#include "ale_controller.h"
#include "vsettings.h"
#include "vbyte_buffer_view.h"
 
AleController::AleController( const vsettings &settings )
    :   _ale( std::unique_ptr<Ale>(new Ale(settings)) ),
        _is_active(false),
        _is_initialized(false),
        _block_di(false)
{
    _ale->connect_indication.   connect(this, &AleController::onAleConnectIndication    );
    _ale->connect_confirmation. connect(this, &AleController::onAleConnectConfirmation  );
    _ale->disconnect_indication.connect(this, &AleController::onAleDisconnectIndication );
    _ale->data_indication.      connect(this, &AleController::onAleDataIndication       );

    _ale->report_tsn.connect(this, &AleController::onAleMessageSending       );

    _is_primary = settings.subgroup("ALE_CONTROLLER").get<bool>("is_primary");
    _master_is_alive = (_is_primary) ? false : true;

    std::string addr = settings.subgroup("ALE_CONTROLLER").get("host_address");
    auto port        = settings.subgroup("ALE_CONTROLLER").get<uint16_t>("host_port");

    _remote_socket.bind( addr, port );

    addr = settings.subgroup("ALE_CONTROLLER").get("remote_address");
    port = settings.subgroup("ALE_CONTROLLER").get<uint16_t>("remote_port");

    _remote_address.address = addr;
    _remote_address.port    = port;

    _init_timer.timeout = [this]()
    {
        this->_is_initialized = this->_is_active = true;
        _master_is_alive = (this->_is_primary) ? true : false;
        vdeb << "Another KVF is out of line: counting TSN from 0";
        doDeferredRequests();

        if (this->_is_primary)
            this->_alive_timer.start( std::chrono::seconds(2) );

    };

    _remote_socket.ready_read.connect([this]()
    {
        vbyte_buffer pkt = _remote_socket.receive();
        if ( this->_is_primary )
        {
            // Основной полукомплект
            if ( this->_master_is_alive )
            {
                // Нормальная работа основного полукомплекта
                if (! _host_received_flag)
                {
                    this->parseRemoteResponse(pkt);
                    _remote_received_flag = true;
                }
            }
            else
            {
                // Ожидание инициализации основного полукомплекта
                this->parseRemoteResponse(pkt);
                this->_alive_timer.start( std::chrono::seconds(2) );
            }

        }
        else
        {
            // Резервный полукомплект
            if( !this->_master_is_alive )
            {
                this->_master_is_alive = true;
                this->_is_active       = false;
                vdeb << "Master is on line";
            }
            else
            {
                if (!_is_initialized)
                {
                    _init_timer.stop();
                    this->_is_initialized = true;
                    this->_is_active = false;

                    if (_connect_request_storage.empty()) return;

                    auto request_data = _connect_request_storage.front();
                    _ale->connectRequest ( request_data.address_type,
                                           request_data.network_address,
                                           request_data.called_etcsid,
                                           request_data.calling_etcsid,
                                           request_data.application_type,
                                           request_data.q_of_s,
                                           request_data.user_data );
                    return;
                }
            }

            this->_alive_timer.stop();
            this->_alive_timer.start( std::chrono::seconds(3) );

            this->parseRemoteRequest(pkt);
         }
    });

    if (_is_primary)
    {
        _alive_timer.timeout = [this]()
        {
            vbyte_buffer buffer;

            buffer.append_BE( toUtype(RequestType::KeepAliveRequest) );
            this->_remote_socket.send_to(buffer, this->_remote_address.address, this->_remote_address.port);
        };

        // Для основного полукомплекта - запрос из резервного TSN по всем сокетным парам
        vbyte_buffer buffer;
        buffer.append_BE( toUtype(RequestType::TsnRequest));
        _remote_socket.send_to(buffer, this->_remote_address.address, this->_remote_address.port);
    }
    else
    {
        // Для резервного полукомплекта - линковка обработчика таймера ожидания пакетов от основного
        _alive_timer.timeout = [this]()
        {
            vdeb << "Master is out of line: become active...";
            _alive_timer.stop();
            _master_is_alive = false;
            _is_active = true;
        };
    }

    _init_timer.singleshot(std::chrono::seconds(3));
}

// Запрос на установление соединения
void AleController::connectRequest (   uint32_t             address_type,
                                        const std::string&  network_address,
                                        uint32_t            called_etcsid,
                                        uint32_t            calling_etcsid,
                                        uint8_t             application_type,
                                        uint8_t             qos,
                                        const std::string&  user_data           )
{
    if (!_is_initialized && _connect_request_storage.empty())

    {
        _connect_request_storage.emplace( address_type,
                                          std::string(network_address),
                                          called_etcsid,
                                          calling_etcsid,
                                          application_type,
                                          qos,
                                          std::string(user_data)            );
        return;
    }

    if (_is_active)
    {
        _host_received_flag = _remote_received_flag = false;

        _ale->connectRequest(address_type, network_address, called_etcsid, calling_etcsid, application_type, qos, user_data);
    }

}

// Ответ на запрос на установление соединения
void AleController::connectResponse( uint16_t            tcepida,
                                     uint32_t            responding_etcsid,
                                     const std::string&  user_data           )
{
    if (_is_active)
    {
        _host_received_flag = _remote_received_flag = false;

        uint16_t tsn{0};

        try
        {
           tsn = _tsn_book.at(tcepida) + 1;
        }
        catch (std::out_of_range &){}

        if (_is_primary)
        {
            vbyte_buffer buffer;
            buffer.append_BE( toUtype(RequestType::ConnectResponse) );
            buffer.append_BE(tsn);
            buffer.append_BE(tcepida);
            buffer.append_BE(responding_etcsid);
            buffer.append_BE(user_data.size());
            buffer.append(user_data);

            _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
        }

        _ale->connectResponse(tcepida, responding_etcsid, user_data);
    }

}

// Запрос на разрыв соединения
void AleController::disconnectRequest( uint16_t tcepidxmt, const std::string& user_data )
{
    if (_is_active)
    {
        _host_received_flag = _remote_received_flag = false;

        if (_is_primary)
        {
            vbyte_buffer buffer;
            buffer.append_BE( toUtype(RequestType::DisconnectRequest) );
            buffer.append_BE(tcepidxmt);
            buffer.append_BE(user_data.size());
            buffer.append(user_data);

            _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
        }

        // Блокируем disconnect_indication для резервного полукомплекта
        if (!_is_primary) _block_di = true;

        _ale->disconnectRequest(tcepidxmt, user_data);
    }

}

// Запрос на передачу данных
void AleController::dataRequest( uint16_t tcepidxmt, const std::string& user_data )
{
    if (_is_active)
    {
        _host_received_flag = _remote_received_flag = false;

        uint16_t tsn{0};

        try
        {
           tsn = _tsn_book.at(tcepidxmt) + 1;
        }
        catch (std::out_of_range &){}

        if (_is_primary)
        {
            vbyte_buffer buffer;
            buffer.append_BE( toUtype(RequestType::DataRequest) );
            buffer.append_BE(tsn);
            buffer.append_BE(tcepidxmt);
            buffer.append_BE(user_data.size());
            buffer.append(user_data);

            _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
        }

        _ale->dataRequest(tcepidxmt, user_data);
    }

}

const std::string& AleController::getNetworkAddress( uint32_t responding_etcsid )
{
    return _ale->getNetworkAddress(responding_etcsid);
}

 void AleController::onAleConnectIndication( const ConnectIndicationData &data )
 {
     if (_is_active)
     {
         if (!_remote_received_flag)
         {
             _host_received_flag = true;
             connect_indication(data);
         }
     }
     else
     {
         vbyte_buffer buffer;
         buffer.append_BE( toUtype(ResponseType::ConnectIndication) );
         buffer.append_BE(data.application_type);
         buffer.append_BE(data.calling_etcsid);
         buffer.append_BE( data.network_address.size() );
         buffer.append(data.network_address);
         buffer.append_BE(data.q_of_s);
         buffer.append_BE(data.tcepida);
         buffer.append_BE( data.user_data.size() );
         buffer.append(data.user_data);

         _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
     }
 }

 void AleController::onAleConnectConfirmation( const ConnectConfirmationData &data )
 {
     if (_is_active)
     {
        if (!_remote_received_flag)
        {
            _host_received_flag = true;
            connect_confirmation(data);
        }
     }
     else
     {
         vbyte_buffer buffer;
         buffer.append_BE( toUtype(ResponseType::ConnectConfirmation) );
         buffer.append_BE(data.responding_etcsid);
         buffer.append_BE(data.tcepidb);
         buffer.append_BE( data.user_data.size() );
         buffer.append(data.user_data);

         _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
     }
 }

 void AleController::onAleDisconnectIndication( const DisconnectIndicationData &data )
 {
     if (_is_active)
     {
         if (_block_di && !_is_primary)
         {
             _block_di = false;
             return;
         }

         if (!_remote_received_flag || !_is_primary)
         {
             vtrace << "AleController: call disconnect_indication";
             _host_received_flag = true;
             disconnect_indication(data);
         }
     }
     else
     {
         vbyte_buffer buffer;
         buffer.append_BE( toUtype(ResponseType::DisconnectIndication) );
         buffer.append_BE(data.reason);
         buffer.append_BE(data.sub_reason);
         buffer.append_BE(data.tcepidrcv);
         buffer.append_BE( data.user_data.size() );
         buffer.append(data.user_data);

         _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
     }
 }

 void AleController::onAleDataIndication( const DataIndicationData &data )
 {
     if (_is_active)
     {
         if (!_remote_received_flag)
         {
             _host_received_flag = true;
             data_indication(data);
         }
     }
     else
     {
         vbyte_buffer buffer;
         buffer.append_BE( toUtype(ResponseType::DataIndication) );
         buffer.append_BE(data.tcepidrcv);
         buffer.append_BE( data.user_data.size() );
         buffer.append(data.user_data);

         _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);
     }
 }

void AleController::onAleMessageSending( uint16_t tcepid, uint16_t tsn )
{
    _tsn_book[tcepid] = tsn;
}

 
void AleController::parseRemoteResponse( const std::string& data )
{
    vbyte_buffer_view buffer_view(data.c_str(), data.size());

    auto resp = static_cast<ResponseType>( buffer_view.BE<int>() );

    switch(resp)
    {
        case ResponseType::ConnectIndication:
        {
            if (_is_primary && _is_active)
            {
                ConnectIndicationData data;

				data.application_type = buffer_view.BE<uint8_t>();
				data.calling_etcsid = buffer_view.BE<uint32_t>();
				auto sz = buffer_view.BE<std::string::size_type>();
				data.network_address = buffer_view.string(sz);
				data.q_of_s = buffer_view.BE<uint8_t>();
                data.tcepida = buffer_view.BE<uint16_t>();
                sz = buffer_view.BE<std::string::size_type>();
                data.user_data = buffer_view.string(sz);

                connect_indication(data);
            }
            break;
        }

        case ResponseType::ConnectConfirmation:
        {
            if (_is_primary && _is_active)
            {
                ConnectConfirmationData data;

				data.responding_etcsid = buffer_view.BE<uint32_t>();
                data.tcepidb = buffer_view.BE<uint16_t>();
                auto sz = buffer_view.BE<std::string::size_type>();
                data.user_data = buffer_view.string(sz);

                connect_confirmation(data);
            }
            break;
        }

        case ResponseType::DisconnectIndication:
        {
            if (_is_primary && _is_active)
            {
                auto Reason  = buffer_view.BE<uint16_t>();
                auto SubReason = buffer_view.BE<uint16_t>();
				auto TCEPIDRCV = buffer_view.BE<uint16_t>();
                auto sz = buffer_view.BE<std::string::size_type>();
                std::string UserData;

                if (sz)
                    UserData = buffer_view.string(sz);

                disconnect_indication( DisconnectIndicationData(TCEPIDRCV, Reason, SubReason, UserData) );
            }
            break;
        }

        case ResponseType::DataIndication:
        {
            if (_is_primary && _is_active)
            {
                DataIndicationData data;

                data.tcepidrcv = buffer_view.BE<uint16_t>();
                auto sz = buffer_view.BE<std::string::size_type>();
                data.user_data = buffer_view.string(sz);

                data_indication(data);
            }
            break;
        }

        case ResponseType::TsnIndication:
        {
            if (_is_primary && !_is_active)
            {
                _init_timer.stop();
                _master_is_alive = _is_active = true;

                // парсинг мапы TSN
                auto sz = buffer_view.BE<decltype(_tsn_book.size())>();
                for (uint32_t i = 0; i < sz; ++i)
                {
                    auto tcepid = buffer_view.BE<uint16_t>();
                    auto tsn =    buffer_view.BE<uint16_t>();
                    vdeb << "get TSN " << tsn << "for TCEPID " << tcepid;
                    _tsn_book.insert(std::pair<uint16_t, uint16_t>(tcepid, tsn));
                    _ale->getPair(tcepid)->tsn[AleTcpSocket::NR_FLAG::N] = tsn + 1;
                    _ale->getPair(tcepid)->tsn[AleTcpSocket::NR_FLAG::R] = tsn + 1;
                }

                this->_is_initialized = true;
                doDeferredRequests();
            }
            break;
        }
    }
}

void AleController::parseRemoteRequest( const std::string& data )
{
    vbyte_buffer_view buffer_view(data.c_str(), data.size());

    auto req = static_cast<RequestType>( buffer_view.BE<int>() );

    switch(req)
    {
        case RequestType::ConnectRequest:
        {
            auto tsn              = buffer_view.BE<uint16_t>();
            auto address_type     = buffer_view.BE<uint32_t>();
            auto sz               = buffer_view.BE<std::string::size_type>();
            auto network_address  = buffer_view.string(sz);
            auto called_etcsid    = buffer_view.BE<uint32_t>();
            auto calling_etcsid   = buffer_view.BE<uint32_t>();
            auto application_type = buffer_view.BE<uint8_t>();
            auto qos              = buffer_view.BE<uint8_t>();
            sz                    = buffer_view.BE<std::string::size_type>();
            auto user_data        = buffer_view.string(sz);

            network_address = _ale->getNetworkAddress(called_etcsid);

            _ale->setTsn(max_num_of_server_connections, tsn);
            _ale->connectRequest(address_type, _ale->getNetworkAddress(called_etcsid), called_etcsid, calling_etcsid, application_type, qos, user_data);
            break;
        }

        case RequestType::ConnectResponse:
        {
            ConnectConfirmationData data;

            auto tsn  = buffer_view.BE<uint16_t>();
            auto tcepida = buffer_view.BE<uint16_t>();
            auto responding_etcsid = buffer_view.BE<uint32_t>();
            auto sz = buffer_view.BE<std::string::size_type>();
            auto user_data = buffer_view.string(sz);

            _ale->setTsn(tcepida, tsn);
            _ale->connectResponse(tcepida, responding_etcsid, user_data);
            break;
        }

        case RequestType::DataRequest:
        {
            auto tsn  = buffer_view.BE<uint16_t>();
            auto tcepidxmt = buffer_view.BE<uint16_t>();
            auto sz = buffer_view.BE<std::string::size_type>();
            std::string user_data = buffer_view.string(sz);

            _ale->setTsn(tcepidxmt, tsn);
            _ale->dataRequest(tcepidxmt, user_data);
            break;
        }

        case RequestType::DisconnectRequest:
        {
            auto tcepidxmt = buffer_view.BE<uint16_t>();
            auto sz = buffer_view.BE<std::string::size_type>();
            std::string user_data = buffer_view.string(sz);

            vdeb << tcepidxmt << " " << user_data;

            _ale->disconnectRequest(tcepidxmt, user_data);
            break;
        }

        case RequestType::TsnRequest:
        {
            //отправка мапы TSN
            vbyte_buffer buffer;
            buffer.append_BE( toUtype(ResponseType::TsnIndication) );

            buffer.append_BE( _tsn_book.size() );
            for (const auto &pr: _tsn_book)
            {
                buffer.append_BE(pr.first);
                buffer.append_BE(pr.second);
                _ale->disconnectRequest(pr.first, "");
            }

            _remote_socket.send_to(buffer, _remote_address.address, _remote_address.port);

            _alive_timer.stop();
            _init_timer.singleshot(std::chrono::seconds(3));
            _is_initialized = false;
            break;
        }

        case RequestType::KeepAliveRequest:
        {
            break;
        }
    }
}

void AleController::doDeferredRequests()
{
        if ( _connect_request_storage.empty() ) return;

        auto request_data = _connect_request_storage.front();

        connectRequest ( request_data.address_type,
                         request_data.network_address,
                         request_data.called_etcsid,
                         request_data.calling_etcsid,
                         request_data.application_type,
                         request_data.q_of_s,
                         request_data.user_data );
}

