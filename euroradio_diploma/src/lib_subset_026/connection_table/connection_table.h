#ifndef CONNECTION_TABLE_H
#define CONNECTION_TABLE_H

#include <map>

#include "vsignal.h"
#include "vtimer.h"
#include "vlog.h"

#include "subset_interface.h"
#include "pending_cs_init.h"

namespace euroradio
{

namespace subset_026
{

class ConnectionTable
{
public:
    ConnectionTable(int wait_time, subset_026::IConnectionController &subset);
    static const uint16_t NO_TCEPID = 101;

    enum class State
    {
        IDLE = 0,
        WAIT_FOR_CON_RESP = 1,
        WAIT_FOR_DATA = 2
    };

    struct ConnParams
    {
        ConnParams(const int32_t number,
                   const int32_t brus_id,
                   const uint32_t etcsid,
                   const State state)
            : number(number),
              brus_id(brus_id),
              tcepid(NO_TCEPID),
              peer_etcsid(etcsid),
              state(state)
        {}

        ConnParams(const uint16_t tcepid,
                   const uint32_t etcsid)
            : tcepid(tcepid),
              peer_etcsid(etcsid)
        {}

        ConnParams()
        {}

        int32_t number = 0;
        int32_t brus_id = 0; // Для БРУСа - собственный БРУС ID; Для РБЦ - ID подключенного БРУСа
        uint16_t tcepid = 0;
        uint32_t peer_etcsid = 0;
        uint32_t last_t_train = 0;

        State state = State::IDLE;
    };

public:
    void insertConnection(const uint32_t peer_etcsid,
                          const ConnParams &parameters);
    bool removeConnection(const uint16_t tcepid);
    bool updateConnectionState(const uint16_t tcepid);
    bool confirmConnection(const uint16_t tcepid);

    void setNoCsInit(bool no_cs_init);
    bool getNoCsInit() const;

    ConnParams *getParametersByTcepid(const uint16_t tcepid);
    ConnParams *getParametersByPeerEtcsid(const uint32_t rbc_id);
    ConnParams *getParametersByBrus(const uint32_t brus_id);
    std::map<uint32_t, ConnParams> getParametersMap();

    void insertPendingCsInitMsg(const uint32_t peer_etcsid, const subset_026::InitiationCSMsg &msg);
    void removePendingCsInitMsg(const uint32_t peer_etcsid);
    std::shared_ptr<subset_026::InitiationCSMsg> getPendingCsInitMsg(const uint32_t peer_etcsid) const;

private:
    class ConnectionParameters
    {
    public:
        ConnParams *getByTcepid(const uint16_t tcepid);
        ConnParams *getByBrus(const uint32_t brus_id);
        ConnParams *getByPeerEtcsid(const uint32_t rbc_id);
        void insert(const ConnParams &params);
        bool remove(const uint16_t tcepid);

        std::map<uint32_t, ConnParams> returnParams();
    private:
        std::map<uint32_t, ConnParams> _params;
    };

    class WatchdogTimers
    {
    public:
        WatchdogTimers(const int wait_time, subset_026::IConnectionController &subset, ConnectionTable &table);

        void set(const uint32_t peer_etcsid);
        void stop(const uint32_t peer_etcsid);
        void unlink(const uint32_t peer_etcsid);
        bool exists(const uint32_t peer_etcsid) const;
        void restart(const uint32_t peer_etcsid);
        void insert(const uint32_t peer_etcsid);
    private:
        int _wait_time = 0;
        std::map<uint32_t, std::shared_ptr<VTimer>> _watchdog_timers;
        subset_026::IConnectionController &_subset;
        ConnectionTable &_table;
    };

private:
    ConnectionParameters _parameters;
    std::shared_ptr<WatchdogTimers> _watchdog_timers;
    PendingCSInit _pending_cs_init_messages;

    bool _no_csinit = false;
};

}

}

#endif //CONNECTION_TABLE_H