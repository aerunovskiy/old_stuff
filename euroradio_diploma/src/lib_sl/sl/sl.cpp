#include "sl.h"
#include "sa_pdu_builder.h"
#include "sl_utils.h"

#include "vsettings.h"

Sl::Sl(const vsettings &settings)
{
    auto _settings = settings.subgroup("SL");

    app_type         = _settings.get<int>     ("appType");
    t_estab          = _settings.get<uint32_t>("testab");
    _etcsid          = _settings.get<uint32_t>("etcsid");
    _ety             = _settings.get<int>     ("ety");
    _connection_type = _settings.get<int>     ("connection_type");

    switch (_connection_type)
    {
    case 0:
        _ale = std::make_shared<AleController>(settings);
        break;
    case 1:
        _ale = std::make_shared<Ale>(settings);
        break;
    default:
        _ale = std::make_shared<Ale>(settings);
        break;
    }
    _parser = std::make_shared<SaPduParser>(_ale);
    
    _parser->received_au1.  connect    (this, &Sl::onAu1Received );
    _parser->received_au2.  connect    (this, &Sl::onAu2Received );
    _parser->received_au3.  connect    (this, &Sl::onAu3Received );
    _parser->received_ar.   connect     (this, &Sl::onArReceived  );
    _parser->received_dt.   connect     (this, &Sl::onDtReceived  );
    _parser->received_di.   connect     (this, &Sl::onDiReceived  );
    _parser->disconnect_indication.connect (this, &Sl::onDisconnectIndication);

    _ale->active_channel.connect(this, &Sl::onActiveChannel);
}

void Sl::connectRequest(uint32_t responding_etcsid )
{
    if ( _states.find(responding_etcsid) == _states.end() )
    {
        _states[responding_etcsid] = IDLE;
    }
    else if ( _states[responding_etcsid] != IDLE)
    {
        vwarning << "SL: Can't establish connection with ETCSID" << responding_etcsid
                 << "connection already exists. State -" << convertStateToString(_states[responding_etcsid]);;
        return;
    }

    _wait_t_estab[responding_etcsid] = static_cast<uint32_t>
    (VSteadyTimePoint::now().milliseconds().count());

    if (_wait_timers.find(responding_etcsid) == _wait_timers.end())
    {
        vdeb << "SL: Place AU2 wait timer for connection with ETCSID" << responding_etcsid;
        _wait_timers.insert(std::pair<uint32_t, std::shared_ptr<VTimer>>(responding_etcsid, std::make_shared<VTimer>()));
    }
    else
    {
        _wait_timers[responding_etcsid]->stop();
    }

    auto timer = _wait_timers[responding_etcsid];

    if ( timer == nullptr )
    {
        vwarning << "SL: Can't set timer for connection with ETCSID" << responding_etcsid << "- timer is nullptr";
        _wait_t_estab.erase(responding_etcsid);
        return;
    }

    timer->timeout.unlink();
    timer->timeout = [this, responding_etcsid]()
    {
        if (_wait_timers.find(responding_etcsid) != _wait_timers.end())
            _wait_timers[responding_etcsid]->stop();

        vrunlog << "SL: SaPDU AU2 have not been received from ETCSID" << responding_etcsid
                << "for more than" << t_estab << "ms. Disconnecting.";

        _wait_rb.erase(responding_etcsid);
        _wait_t_estab.erase(responding_etcsid);

        disconnectRequest(Ale::max_num_of_server_connections, 7, 3);
    };

    _wait_timers[responding_etcsid]->start(std::chrono::milliseconds (t_estab));

    _wait_rb[responding_etcsid] = static_cast<uint64_t>(rand());

    Au1SaPdu au1(_ety, _etcsid, _wait_rb[responding_etcsid]);

    vdeb << "SL: Посылка пакета SaPDU AU1...";

    _con_params.insert(std::pair<uint16_t, SlConnectParams>(CLIENT_TCEPID, SlConnectParams()));

    _con_params.at(CLIENT_TCEPID).df           = DF::TO_RESP;
    _con_params.at(CLIENT_TCEPID).peer_etcsid  = responding_etcsid;

   _ale->connectRequest( _ety,
                       this->_ale->getNetworkAddress(responding_etcsid),
                       responding_etcsid,
                       _etcsid,
                       app_type,
                       q_of_service,
                       SaPduBuilder::formAu1(au1) );

    _states[responding_etcsid] = WFTC;
}

void Sl::connectResponse(uint16_t tcepid)
{
    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0) return;

    if ( _states[etcsid] == WFRESP)
    {
        ArSaPdu ar( _con_params.at(tcepid).peer_etcsid,
                    _con_params.at(tcepid).ra,
                    _con_params.at(tcepid).rb );

        vdeb << "SL: Посылка пакета SaPDU AR...";

        _ale->dataRequest( tcepid,
                        SaPduBuilder::formAr(ar) );

        _states[etcsid] = DATA;
    }
    else
    {
        vwarning << "SL: Can't send ConnectResponse. Status not WFRESP but"
                 << convertStateToString(_states[etcsid]);
    }
}

void Sl::dataRequest(uint16_t tcepid, std::string user_data)
{
    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0) return;

    if (_states[etcsid] == DATA)
    {
        vbyte_buffer data(user_data);
        DtSaPdu dt( _con_params.at(tcepid).df,
                     data,
                     _con_params.at(tcepid).peer_etcsid,
                     _con_params.at(tcepid).ra,
                     _con_params.at(tcepid).rb );

        vdeb << "SL: Посылка пакета SaPDU DT...";

        std::string sa_pdu_dt = SaPduBuilder::formDt(dt);

        _ale->dataRequest(tcepid, sa_pdu_dt);
    }
}

void Sl::disconnectRequest( const uint16_t tcepid, uint8_t reason, uint8_t sub_reason )
{
    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0)
    {
        vwarning << "SL: Can't disconnect from TCEPID" << tcepid << "- no corresponding ETCSID.";
        return;
    }

    if (_states.find(etcsid) == _states.end())
    {
        vwarning << "SL: Can't disconnect from ETCSID" << etcsid << "- no corresponding state.";
        return;
    }

    if (_states[etcsid] == IDLE)
    {
        vwarning << "SL: Can't disconnect from ETCSID" << etcsid << "- state = " << convertStateToString(_states[etcsid]);
        return;
    }

    if (_con_params.find(tcepid) == _con_params.end())
    {
        vwarning << "SL: Can't disconnect from ETCSID" << etcsid << "- state = " << convertStateToString(_states[etcsid])
                 << ": connection parameters for ETCSID have not been found";
        return;
    }

    if (_con_params.at(tcepid).timer_estab == nullptr)
    {
        vwarning << "SL: Can't disconnect from ETCSID" << etcsid << "- state = " << convertStateToString(_states[etcsid])
                 << ": can't find timer";
        return;
    }

    _con_params.at(tcepid).timer_estab->stop();
    _con_params.at(tcepid).timer_estab->timeout.unlink();

    DiSaPdu di(_con_params.at(tcepid).df, reason, sub_reason);

    _con_params.erase(tcepid);

    vdeb << "SL: Посылка пакета SaPDU DI...";

    _ale->disconnectRequest(tcepid, SaPduBuilder::formDi(di));

    DisRepIndicationData data(tcepid, reason, sub_reason);

    Ale::DisconnectIndicationData dis(tcepid, 0, 0);

    _states[etcsid] = IDLE;

    disconnect_indication(data, dis);

}

void Sl::disconnectByPeer(  uint32_t peer_etcsid,
                            uint8_t  reason,
                            uint8_t  sub_reason  )
{
    const uint16_t tcepid = getTcepidByEctsid(peer_etcsid);
    if (tcepid != 101)
    {
        disconnectRequest(tcepid, reason, sub_reason);
    }
    else
    {
        if ( _wait_timers.find(peer_etcsid) != _wait_timers.end() )
        {
            _wait_timers.at(peer_etcsid)->stop();
            _wait_timers.at(peer_etcsid)->timeout.unlink();
        }

        _wait_t_estab.erase(peer_etcsid);

        _wait_rb.erase(peer_etcsid);

        _states[peer_etcsid] = IDLE;
    }
}

void Sl::onAu1Received(Au1SaPdu au1, uint16_t tcepid, uint32_t Calling_etcsid)
{
    vdeb << "SL: Получен пакет SaPDU AU1";

    if (_states.find(Calling_etcsid) == _states.end())
    {
        _states[Calling_etcsid] = IDLE;
    }

    if (_states[Calling_etcsid] == IDLE)
    {
        if (au1.sasaf.field.saf != SaPduParser::saf)
        {
            uint16_t saf_temp = SaPduParser::saf;
            vrunlog << "SL: TCEPID" << tcepid << "SaF in AU1 =" << static_cast<uint16_t>(au1.sasaf.field.saf) << "but must be" << saf_temp << "Disconnect";
            disconnectRequest(tcepid, 3, 29);
            return;
        }

        if (au1.header.field.df != TO_RESP)
        {
            vrunlog << "SL: TCEPID" << tcepid << "DF in AU1 =" << static_cast<uint16_t>(au1.header.field.df)
                    << "but must be" << static_cast<uint16_t>(TO_RESP) << "Disconnect";

            disconnectRequest(tcepid, 6, 2);
            return;
        }

        if (au1.header.field.mti != MTI::SA_AU1)
        {
            vrunlog << "SL: TCEPID" << tcepid << "MTI in AU1 =" << static_cast<uint16_t>(au1.header.field.mti)
                    << "but must be" << static_cast<uint16_t>(MTI::SA_AU1) << "Disconnect";

            disconnectRequest(tcepid, 127, 0);
            return;
        }

        if (au1.header.field.ety != ETY::INTERLOCKING && au1.header.field.ety != ETY::ENGINE)
        {
            vrunlog << "SL: TCEPID" << tcepid << "ETY in AU1 =" << static_cast<uint16_t>(au1.header.field.ety)
                    << "but must be" << static_cast<uint16_t>(ETY::INTERLOCKING) << "or" << static_cast<uint16_t>(ETY::ENGINE) << "Disconnect";

            disconnectRequest(tcepid, 127, 0);
            return;
        }

        if (_con_params.find(tcepid) != _con_params.end())
            _con_params.erase(tcepid);

        _con_params.insert(std::pair<uint16_t, SlConnectParams>(tcepid, SlConnectParams()));

        _con_params.at(tcepid).df           = DF::TO_INIT;
        _con_params.at(tcepid).peer_etcsid  = au1.sasaf.field.sa;
        _con_params.at(tcepid).rb           = au1.rb;
        _con_params.at(tcepid).ra           = static_cast<uint64_t>(rand());

        Au2SaPdu au2(_ety, _etcsid, _con_params.at(tcepid).ra, au1.rb, Calling_etcsid);

        vdeb << "SL: Посылка пакета SaPDU AU2...";

        ale_connect_indication(tcepid, Calling_etcsid);

        _ale->connectResponse( tcepid,
                            _etcsid,
                            SaPduBuilder::formAu2(au2) );

        _states[Calling_etcsid] = WFAU3;
    }
    else
    {
        vrunlog << "SL: Received AU1: State of connection with ETCSID should be IDLE but is" << convertStateToString(_states[Calling_etcsid]);
    }
}

void Sl::onAu2Received(Au2SaPdu au2, uint16_t tcepid)
{    
    vdeb << "SL: Получен пакет SaPDU AU2";

    if (_states.find(au2.sasaf.field.sa) == _states.end())
    {
        vrunlog << "SL: Received AU2: Can't find state of connectiond with" << static_cast<uint32_t>(au2.sasaf.field.sa);
        return;
    }

    if (_states[au2.sasaf.field.sa] == WFTC)
    {
        if (_con_params.find(tcepid) != _con_params.end())
            _con_params.erase(tcepid);

        _con_params.insert(std::pair<uint16_t, SlConnectParams>(tcepid, SlConnectParams()));

        _con_params.at(tcepid).ra = au2.ra;
        _con_params.at(tcepid).rb = _wait_rb[au2.sasaf.field.sa];
        _con_params.at(tcepid).df = DF::TO_RESP;
        _con_params.at(tcepid).peer_etcsid = au2.sasaf.field.sa;

        if (au2.sasaf.field.saf != SaPduParser::saf)
        {
            uint16_t saf_temp = SaPduParser::saf;
            vrunlog << "SL: TCEPID" << tcepid << "SaF in AU2 =" << static_cast<uint16_t>(au2.sasaf.field.saf)
                    << "but must be" << saf_temp << "Disconnect";

            disconnectRequest(tcepid, 3, 29);
            return;
        }

        if (au2.header.field.df != TO_INIT)
        {
            vrunlog << "SL: TCEPID" << tcepid << "DF in AU1 =" << static_cast<uint16_t>(au2.header.field.df)
                    << "but must be" << static_cast<uint16_t>(TO_INIT) << "Disconnect";

            disconnectRequest(tcepid, 6, 1);
            return;
        }

        if (au2.header.field.mti != SA_AU2)
        {
            vrunlog << "SL: TCEPID" << tcepid << "MTI in AU1 =" << static_cast<uint16_t>(au2.header.field.mti)
                    << "but must be" << static_cast<uint16_t>(MTI::SA_AU2) << "Disconnect";

            disconnectRequest(tcepid, 9, 1);
            return;
        }

        if (au2.header.field.ety != ETY::INTERLOCKING && au2.header.field.ety != ETY::RBC)
        {
            vrunlog << "SL: TCEPID" << tcepid << "ETY in AU2 =" << static_cast<uint16_t>(au2.header.field.ety)
                    << "but must be" << static_cast<uint16_t>(ETY::INTERLOCKING) << "or" << static_cast<uint16_t>(ETY::RBC) << "Disconnect";

            disconnectRequest(tcepid, 127, 0);
            return;
        }


        if (_wait_timers.find(au2.sasaf.field.sa) != _wait_timers.end())
        {
            vdeb << "SL: Stop timer" << static_cast<uint32_t>(au2.sasaf.field.sa);

            _wait_timers[static_cast<uint32_t>(au2.sasaf.field.sa)]->stop();
            _wait_timers[static_cast<uint32_t>(au2.sasaf.field.sa)]->timeout.unlink();
        }

        _wait_rb.erase(static_cast<uint32_t>(au2.sasaf.field.sa));

        if (_wait_t_estab.find(static_cast<uint32_t>(au2.sasaf.field.sa)) == _wait_t_estab.end())
        {
            vrunlog << "SL: TCEPID" << tcepid << "Received AU2: Can't find timestamp of send SaPDU AU1 for ETCSID" << static_cast<uint32_t>(au2.sasaf.field.sa);

            //disconnectRequest(tcepid, 7, 3);
            //return;
        }

        uint32_t diff =
                static_cast<uint32_t>( VSteadyTimePoint::now().milliseconds().count() ) -
                _wait_t_estab[static_cast<uint32_t>(au2.sasaf.field.sa)];

        _wait_t_estab.erase(static_cast<uint32_t>(au2.sasaf.field.sa));

        if (diff > t_estab)
        {
           vrunlog << "SL: TCEPID" << tcepid << "Received AU2: Connection establishment time ("<< diff
                    << ") exceeded allowed" << t_estab << "Disconnecting";

           disconnectRequest(tcepid, 7, 3);
           return;
        }
        else
        {
            // При запросе мы не знаем tcepid, приходится считать сначала VTimePoint
            _con_params.at(tcepid).timer_estab->start(
                        std::chrono::milliseconds(t_estab - diff) );

            _con_params.at(tcepid).timer_estab->timeout = [this, tcepid]()
            {
                uint32_t etcsid = getEtcsidByTcepid(tcepid);
                if ( etcsid == 0) return;

                if (_states[etcsid] == WFTC ||
                    _states[etcsid] == WFAR )
                {
                    vrunlog << "SL: Received AU2: state of connection with ETCSID"
                            << etcsid << "is not in WFAU2 but" << convertStateToString(_states[etcsid]) << ". Disconnect";

                    disconnectRequest(tcepid, 7, 3);
                    return;
                }
            };
        }

        _con_params.at(tcepid).timer_estab->start( std::chrono::milliseconds() );

        if ( au2.calcMac( _con_params.at(tcepid).ra,
                          _con_params.at(tcepid).rb,
                          _etcsid                     ) != au2.mac )
        {            
            vrunlog << "SL: MAC calulation error in SaPDU AU2 from ETCSID"
                    << static_cast<uint32_t>(au2.sasaf.field.sa) << ". Disconnecting.";

            disconnectRequest(tcepid, 4, 2);
            return;
        }
        else
        {

            Au3SaPdu au3(_con_params.at(tcepid).ra,
                          _con_params.at(tcepid).rb,
                          _con_params.at(tcepid).peer_etcsid);

            vdeb << "SL: Посылка пакета SaPDU AU3...";

            ale_connect_confirmation(tcepid, _con_params.at(tcepid).peer_etcsid);

            _ale->dataRequest(tcepid, SaPduBuilder::formAu3(au3));
            _states[au2.sasaf.field.sa] = WFAR;
        }
    }
    else
    {
        vrunlog << "SL: Received AU2: State of connection with ETCSID" << static_cast<uint16_t>(au2.sasaf.field.sa)
                << "should be WFAU2 but is" << convertStateToString( _states[au2.sasaf.field.sa] );
    }
}

void Sl::onAu3Received(Au3SaPdu au3, uint16_t tcepid)
{
    vdeb << "SL: Получен пакет SaPDU AU3";

    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0)
    {
        vrunlog << "SL: Received AU3: Can't find ETCSID for TCEPID =" << tcepid;

        return;
    }

    if (_states[etcsid] == WFAU3)
    {
        if (au3.header.field.df != TO_RESP)
        {
            vrunlog << "SL: TCEPID" << tcepid << "DF in AU3 =" << static_cast<uint16_t>(au3.header.field.df)
                    << "but must be" << static_cast<uint16_t>(TO_RESP) << "Disconnect";

            disconnectRequest(tcepid, 6, 2);
            return;
        }

        if (au3.header.field.mti != SA_AU3)
        {
            vrunlog << "SL: TCEPID" << tcepid << "MTI in AU3 =" << static_cast<uint16_t>(au3.header.field.mti)
                    << "but must be" << static_cast<uint16_t>(MTI::SA_AU3) << "Disconnect";

            disconnectRequest(tcepid, 9, 2);
            return;
        }

        if (au3.header.field.ety != 0)
        {
            vrunlog << "SL: TCEPID" << tcepid << "ETY in AU3 =" << static_cast<uint16_t>(au3.header.field.ety)
                    << "but must be" << static_cast<uint16_t>(ETY::INTERLOCKING) << "or" << static_cast<uint16_t>(ETY::ENGINE) << "Disconnect";

            disconnectRequest(tcepid, 127, 0);
            return;
        }

        if (au3.calcMac(_con_params.at(tcepid).ra,
                        _con_params.at(tcepid).rb, _etcsid) != au3.mac)
        {
            vrunlog << "SL: MAC calulation error in SaPDU AU3 from ETCSID"
                    << static_cast<uint32_t>(etcsid) << ". Disconnecting.";

            disconnectRequest(tcepid, 4, 3);
            return;
        }
        else
        {
            _states[etcsid] = WFRESP;
            connect_indication(tcepid);
        }
    }
    else
    {
        vrunlog << "SL: Received AU3: State of connection with ETCSID should be WFAU3 but is" << convertStateToString(_states[etcsid]);
    }

}

void Sl::onArReceived(ArSaPdu ar, uint16_t tcepid)
{
    vdeb << "SL: Получен пакет SaPDU AR";

    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0)
    {
        vrunlog << "SL: AR received. Can't find ETCSID for TCEPID" << tcepid;
        return;
    }

    if (_states[etcsid] == WFAR)
    {

        if (ar.header.field.df != TO_INIT)
        {
            vrunlog << "SL: TCEPID" << tcepid << "DF in AR =" << static_cast<uint16_t>(ar.header.field.df)
                    << "but must be" << static_cast<uint16_t>(TO_INIT) << "Disconnect";

            disconnectRequest(tcepid, 6, 1);
            return;
        }

        if (ar.header.field.mti != SA_AR)
        {
            vrunlog << "SL: TCEPID" << tcepid << "MTI in AR =" << static_cast<uint16_t>(ar.header.field.mti)
                    << "but must be" << static_cast<uint16_t>(MTI::SA_AR) << "Disconnect";

            disconnectRequest(tcepid, 9, 3);
            return;
        }

        if (ar.header.field.ety != 0)
        {
            vrunlog << "SL: TCEPID" << tcepid << "ETY in AR =" << static_cast<uint16_t>(ar.header.field.ety)
                    << "but must be 0. Disconnect";

            disconnectRequest(tcepid, 127, 0);
            return;
        }

        if (ar.calcMac(_etcsid,
                       _con_params.at(tcepid).ra,
                       _con_params.at(tcepid).rb) != ar.mac)
        {
            vrunlog << "SL: MAC calulation error in SaPDU AR from ETCSID"
                    << static_cast<uint32_t>(etcsid) << ". Disconnecting.";

            disconnectRequest(tcepid, 4, 4);
            return;
        }
        else
        {
            _con_params.at(tcepid).timer_estab->stop();
            _states[etcsid] = DATA;
            connect_confirmation(tcepid);
        }
    }
    else
    {
        vrunlog << "SL: Received AR: State of connection with ETCSID should be WFAR but is" << convertStateToString(_states[etcsid]);
    }
}

void Sl::onDtReceived(DtSaPdu dt, uint16_t tcepid, uint8_t channel)
{
    vdeb << "SL: Получен пакет SaPDU DT";

    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0) return;

    if (_states[etcsid] == DATA)
    {
        if (_con_params.at(tcepid).df == TO_RESP)
        {
            if (dt.header.field.df != TO_INIT)
            {
                disconnectRequest(tcepid, 6, 1);
                return;
            }
        }
        else
            if (dt.header.field.df != TO_RESP)
            {
                disconnectRequest(tcepid, 6, 1);
                return;
            }

        if (dt.header.field.mti != SA_DT)
        {
            vfatal << "SAI: WRONG MTI (SaPDU DT)";
            disconnectRequest(tcepid, 127, 0);
            return;
        }

        if (dt.calcMac(_etcsid,
                       _con_params.at(tcepid).ra,
                       _con_params.at(tcepid).rb) != dt.mac)
        {
            vfatal << "Неверный MAC в SaPDU. Counter";
            disconnectRequest(tcepid, 4, 1);
            return;
        }

        Sl::DataIndicationData data(tcepid, dt.user_data, channel);
        data_indication(data);
    }
}

void Sl::onDiReceived(DiSaPdu di, Ale::DisconnectIndicationData dt, uint16_t tcepid)
{
    vdeb << "SL: Получен пакет SaPDU DI";

    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0) return;

    _states[etcsid] = IDLE;

    if ( _con_params.find(tcepid) == _con_params.end() )
    {
        vwarning << "SL: Can't process SaPDU - no params for connection with TCEPID" << tcepid;
    }
    else if ( _con_params.at(tcepid).timer_estab == nullptr )
    {
        vwarning << "SL: Can't stop timer for connection with TCEPID" << tcepid
                 << "- timer is nullptr";
    }
    else
    {
        _con_params.at(tcepid).timer_estab->stop();
        _con_params.at(tcepid).timer_estab->timeout.unlink();
    }

    _wait_rb.erase(_con_params.at(tcepid).peer_etcsid);
    _wait_t_estab.erase(_con_params.at(tcepid).peer_etcsid);
    _wait_timers.at(_con_params.at(tcepid).peer_etcsid)->stop();

    _con_params.erase(tcepid);

    Sl::DisRepIndicationData data(tcepid, di.reason, di.sub_reason);
    disconnect_indication(data, dt);
}

void Sl::onDisconnectIndication(Ale::DisconnectIndicationData dt, uint16_t tcepid)
{
    vdeb << "SL: Разрыв соединения на уровне ALE tcepid = " << tcepid;

    uint32_t etcsid = getEtcsidByTcepid(tcepid);
    if (etcsid == 0) return;

    _states[etcsid] = IDLE;

    if ( _con_params.find(tcepid) == _con_params.end() )
    {
        vwarning << "SL: Can't process ALE Disconnect Indication - no params for connection with TCEPID" << tcepid;
    }
    else if ( _con_params.at(tcepid).timer_estab == nullptr )
    {
        vwarning << "SL: Can't stop timer for connection with TCEPID" << tcepid
                 << "- timer is nullptr";
    }
    else
    {
        _con_params.at(tcepid).timer_estab->stop();
        _con_params.at(tcepid).timer_estab->timeout.unlink();
    }

    _wait_rb.erase(_con_params.at(tcepid).peer_etcsid);
    _wait_t_estab.erase(_con_params.at(tcepid).peer_etcsid);
    _wait_timers.at(_con_params.at(tcepid).peer_etcsid)->stop();

    _con_params.erase(tcepid);

    Sl::DisRepIndicationData data(tcepid, 0, 0);
    disconnect_indication(data, dt);
}

void Sl::onActiveChannel(bool channel)
{
    active_channel(channel);
}
