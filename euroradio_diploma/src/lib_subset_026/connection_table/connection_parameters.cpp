#include "connection_table.h"

namespace euroradio
{

namespace subset_026
{

ConnectionTable::ConnParams *ConnectionTable::ConnectionParameters::getByTcepid(const uint16_t tcepid)
{
    for (auto &conn_param : _params)
    {
        if (conn_param.second.tcepid == tcepid)
            return &conn_param.second;
    }

    vwarning << "ConnectionTable: Can't find params with TCEPID" << tcepid;
    return nullptr;
}

ConnectionTable::ConnParams *ConnectionTable::ConnectionParameters::getByBrus(const uint32_t brus_id)
{
    for (auto &conn_param : _params)
    {
        if (conn_param.second.brus_id == brus_id)
            return &conn_param.second;
    }

    vwarning << "ConnectionTable: Can't find params with BRUS ID" << brus_id;
    return nullptr;
}

ConnectionTable::ConnParams *ConnectionTable::ConnectionParameters::getByPeerEtcsid(const uint32_t peer_etcsid)
{
    for (auto &conn_param : _params)
    {
        if (conn_param.second.peer_etcsid == peer_etcsid)
            return &conn_param.second;
    }

    vwarning << "Can't get params with unknown peer ETCSID:" << peer_etcsid;
    return nullptr;
}

void ConnectionTable::ConnectionParameters::insert(const ConnParams &params)
{
    _params.insert(std::make_pair(params.peer_etcsid, params));
}

bool ConnectionTable::ConnectionParameters::remove(const uint16_t tcepid)
{
    auto params = getByTcepid(tcepid);
    if (params != nullptr)
    {
        _params.erase(params->peer_etcsid);
        return true;
    }
    else return false;
}

std::map<uint32_t, ConnectionTable::ConnParams> ConnectionTable::ConnectionParameters::returnParams()
{
    return _params;
}

}

}