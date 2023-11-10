//=======================================================================================
#include "connection_table.h"
//=======================================================================================
ConnectionTable::WatchdogTimers::WatchdogTimers(const int wait_time, subset_026::IConnectionController& subset, ConnectionTable& table)
:_wait_time(wait_time),
 _subset   (subset),
 _table    (table)
{}
//=======================================================================================
void ConnectionTable::WatchdogTimers::set(const uint32_t peer_etcsid)
{
    if ( !exists(peer_etcsid) )
    {
        vwarning << "WatchdogTimer: Can't set watchdog timer for connection with" << peer_etcsid
                 << "- timer not exists.";
        return;
    }

    _watchdog_timers[peer_etcsid]->timeout = [&, peer_etcsid]()
    {
        auto params_by_peer_etcsid = _table.getParametersByPeerEtcsid(peer_etcsid);
        if (params_by_peer_etcsid != nullptr)
        {
            stop(params_by_peer_etcsid->peer_etcsid);

            switch (params_by_peer_etcsid->state)
            {
                case State::IDLE:
                {
                    break;
                }
                case State::WAIT_FOR_CON_RESP:
                {
                    vrunlog << "WatchdogTimer, STATES::WAIT_FOR_CON_RESP: Connect Response is not received! Removing BRUS ID"
                            << params_by_peer_etcsid->brus_id << "data from subset parameters";

                    _subset.disconnectByPeer(params_by_peer_etcsid->peer_etcsid, 0, 0);
                    _table.removeConnection (params_by_peer_etcsid->tcepid);

                    break;
                }
                case State::WAIT_FOR_DATA:
                {
                    vrunlog << "WatchdogTimer, STATES::WAIT_FOR_DATA: Message from ETCSID" << params_by_peer_etcsid->peer_etcsid
                            << "has not been received for more than"
                            << _wait_time << "seconds. Execute Disconnect Request.";
                    _subset.disconnectRequest(_table.getParametersByPeerEtcsid(peer_etcsid)->tcepid, 0 ,0);

                    break;
                }
                default:
                {
                    vrunlog << "WatchdogTimer, default: Unknown state of connection with ETCSD"
                            << params_by_peer_etcsid->peer_etcsid << "Disconnecting.";
                    _subset.disconnectRequest(_table.getParametersByPeerEtcsid(peer_etcsid)->tcepid, 0 ,0);

                    break;
                }
            }
        }
        else
        {
            vdeb << "WatchdogTimer: Params with ETCSID" << peer_etcsid << "are not exist!";
        }
    };

    vdeb << "WatchdogTimer: WatchdogTimer starts for" << _wait_time << "seconds";
    _watchdog_timers[peer_etcsid]->start(std::chrono::seconds(_wait_time));
}
//=======================================================================================
void ConnectionTable::WatchdogTimers::stop(uint32_t peer_etcsid)
{
    auto watchdog_iterator = _watchdog_timers.find(peer_etcsid);
    if ( watchdog_iterator != _watchdog_timers.end() && watchdog_iterator->second != nullptr )
    {
        watchdog_iterator->second->stop();
    }
}
//=======================================================================================
void ConnectionTable::WatchdogTimers::unlink(const uint32_t peer_etcsid)
{
    auto watchdog_iterator = _watchdog_timers.find(peer_etcsid);
    if ( watchdog_iterator != _watchdog_timers.end() && watchdog_iterator->second != nullptr )
    {
        watchdog_iterator->second->stop();
        watchdog_iterator->second->timeout.unlink();
    }
}
//=======================================================================================
bool ConnectionTable::WatchdogTimers::exists(const uint32_t peer_etcsid) const
{
    return (_watchdog_timers.find(peer_etcsid) != _watchdog_timers.end());
}
//=======================================================================================
void ConnectionTable::WatchdogTimers::restart(const uint32_t peer_etcsid)
{
    auto watchdog_iterator = _watchdog_timers.find(peer_etcsid);
    if ( watchdog_iterator != _watchdog_timers.end() && watchdog_iterator->second != nullptr )
    {
        watchdog_iterator->second->stop();
        watchdog_iterator->second->start(std::chrono::seconds(_wait_time));
    }
}
//=======================================================================================
void ConnectionTable::WatchdogTimers::insert(const uint32_t peer_etcsid)
{
    _watchdog_timers.insert(std::pair<uint32_t, std::shared_ptr<VTimer>>(peer_etcsid, std::make_shared<VTimer>()));
}
//=======================================================================================