#ifndef PENDING_CS_INIT_H
#define PENDING_CS_INIT_H

#include <map>

#include "vlog.h"

#include "messages.h"

namespace euroradio
{

namespace subset_026
{

class PendingCSInit
{
public:
    bool exists(const uint32_t peer_etcsid) const;
    void insert(const uint32_t peer_etcsid, const subset_026::InitiationCSMsg &msg);
    void remove(const uint32_t peer_etcsid);
    std::shared_ptr<subset_026::InitiationCSMsg> get(const uint32_t peer_etcsid) const;
private:
    std::map<uint32_t, subset_026::InitiationCSMsg> _messages_map;
};

}

}

#endif //PENDING_CS_INIT_H