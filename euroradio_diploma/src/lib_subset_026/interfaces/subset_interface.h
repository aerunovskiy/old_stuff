#pragma once

namespace euroradio
{

namespace subset_026
{
    class IConnectionController
    {
    public:
        virtual void disconnectByPeer( uint32_t peer_etcsid,
                                       uint8_t  reason,
                                       uint8_t  sub_reason  ) = 0;

        virtual void disconnectRequest( uint16_t tcepidxmt,
                                        uint8_t  reason,
                                        uint8_t  sub_reason) = 0;

    };
}

}