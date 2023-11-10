#include "connection_table.h"

namespace euroradio
{

namespace subset_026
{

ConnectionTable::ConnectionTable(int wait_time, subset_026::IConnectionController &subset)
: _watchdog_timers(std::make_shared<WatchdogTimers>(wait_time, subset, *this))
{}

ConnectionTable::ConnParams *ConnectionTable::getParametersByTcepid(const uint16_t tcepid)
{
    return _parameters.getByTcepid(tcepid);
}

ConnectionTable::ConnParams *ConnectionTable::getParametersByPeerEtcsid(const uint32_t rbc_id)
{
    return _parameters.getByPeerEtcsid(rbc_id);
}

ConnectionTable::ConnParams *ConnectionTable::getParametersByBrus(const uint32_t brus_id)
{
    return _parameters.getByBrus(brus_id);
}

std::shared_ptr<subset_026::InitiationCSMsg> ConnectionTable::getPendingCsInitMsg(const uint32_t peer_etcsid) const
{
    return _pending_cs_init_messages.get(peer_etcsid);
}

void ConnectionTable::removePendingCsInitMsg(const uint32_t peer_etcsid)
{
    _pending_cs_init_messages.remove(peer_etcsid);
}

void ConnectionTable::insertPendingCsInitMsg(const uint32_t peer_etcsid, const subset_026::InitiationCSMsg &msg)
{
    _pending_cs_init_messages.insert(peer_etcsid, msg);
}

std::map<uint32_t, ConnectionTable::ConnParams> ConnectionTable::getParametersMap()
{
    return _parameters.returnParams();
}

void ConnectionTable::insertConnection(const uint32_t peer_etcsid, const ConnectionTable::ConnParams &parameters)
{
    _parameters.insert(parameters);

    if (_watchdog_timers->exists(peer_etcsid))
        _watchdog_timers->unlink(peer_etcsid);
    else
        _watchdog_timers->insert(peer_etcsid);

    _watchdog_timers->set(peer_etcsid);
}

bool ConnectionTable::removeConnection(const uint16_t tcepid)
{
    auto params = _parameters.getByTcepid(tcepid);
    if (params == nullptr)
        return false;

    params->state = ConnectionTable::State::IDLE;

    _watchdog_timers->unlink(params->peer_etcsid);
    _pending_cs_init_messages.remove(params->tcepid);

    return _parameters.remove(params->tcepid);
}

bool ConnectionTable::updateConnectionState(const uint16_t tcepid)
{
    auto params = _parameters.getByTcepid(tcepid);
    if (params != nullptr)
        _watchdog_timers->restart(params->peer_etcsid);

    return (params != nullptr);
}

bool ConnectionTable::confirmConnection(const uint16_t tcepid)
{
    auto params = _parameters.getByTcepid(tcepid);
    if (params != nullptr) {
        if (_no_csinit) {
            vdeb << "ConnectTable: confirm connection without InitiationCsMsg";
            params->state = ConnectionTable::State::WAIT_FOR_DATA;
            _watchdog_timers->restart(params->peer_etcsid);
            setNoCsInit(false);

            return true;
        }

        if (!_pending_cs_init_messages.exists(params->peer_etcsid)) {
            vrunlog << "ConnectionTable: no pending CSInit message for established connection with ETCSID"
                    << params->peer_etcsid << ". Disconnecting.";
            return false;
        }

        auto cs_init_msg = _pending_cs_init_messages.get(params->peer_etcsid);
        if (cs_init_msg == nullptr) {
            vrunlog << "ConnectionTable: Unexpectedly nullptr pending CSInitMsg for established connection with ETCSID"
                    << params->peer_etcsid << ". Disconnecting.";

            _pending_cs_init_messages.remove(params->peer_etcsid);
            return false;
        }

        // Проверка на соответствие параметров соединения и ожидающего сообщения CSInit
        if (cs_init_msg->header.brus_id != params->brus_id) {
            vrunlog << "TableConnection: CSInitMsg BRUS ID" << cs_init_msg->header.brus_id
                    << "not matched with BRUS ID" << params->brus_id << "for established connection with ETCSID"
                    << params->peer_etcsid << ". Disconnecting.";

            _pending_cs_init_messages.remove(params->peer_etcsid);
            return false;
        }

        params->state = ConnectionTable::State::WAIT_FOR_DATA;
        _watchdog_timers->restart(params->peer_etcsid);
        setNoCsInit(false);

        return true;
    }
    else return false;
}

void ConnectionTable::setNoCsInit(bool no_cs_init)
{
    _no_csinit = no_cs_init;
}

bool ConnectionTable::getNoCsInit() const
{
    return _no_csinit;
}

}

}