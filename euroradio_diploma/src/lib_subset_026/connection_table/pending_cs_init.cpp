#include "pending_cs_init.h"

namespace euroradio
{

namespace subset_026
{

bool PendingCSInit::exists(const uint32_t peer_etcsid) const
{
    return (_messages_map.find(peer_etcsid) != _messages_map.end());
}

void PendingCSInit::insert(const uint32_t peer_etcsid, const subset_026::InitiationCSMsg &msg)
{
    if (this->exists(peer_etcsid))
    {
        vwarning << "PendingCSInit: Can't place CSInitMsg to pending for connection with ETCSID"
                 << peer_etcsid << "- CSInit for this ETCSID already exists";
        return;
    }

    _messages_map.insert(std::pair<uint32_t, subset_026::InitiationCSMsg>(peer_etcsid, msg));
    vdeb << "PendingCSInit: Placed pending CSInit for connection with ETCSID" << peer_etcsid;
}

void PendingCSInit::remove(const uint32_t peer_etcsid)
{
    if (!this->exists(peer_etcsid))
    {
        vwarning << "PendingCSInit: Can't remove CSInitMsg with ETCSID" << peer_etcsid
                 << "- CSInit for this ETCSID is not exists";
        return;
    }

    _messages_map.erase(peer_etcsid);
    vdeb << "PendingCSInit: Removed pending CSInit for connection with ETCSID" << peer_etcsid;
}

std::shared_ptr<subset_026::InitiationCSMsg> PendingCSInit::get(const uint32_t peer_etcsid) const
{
    if (!this->exists(peer_etcsid))
    {
        vwarning << "PendingCSInit: Can't found pending CSInitMsg for connection with ETCSID" << peer_etcsid;
        return nullptr;
    }

    return std::make_shared<subset_026::InitiationCSMsg>(_messages_map.at(peer_etcsid));
}

}

}