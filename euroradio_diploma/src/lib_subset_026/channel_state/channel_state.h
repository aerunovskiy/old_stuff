#ifndef CHANNEL_STATE_H
#define CHANNEL_STATE_H

#include <cstdint>

#include "vtimer.h"
#include "vsignal.h"
#include "vlog.h"

namespace euroradio
{

namespace subset_026
{
    class ChannelState
    {
    public:
        ChannelState(uint8_t channel_wait, uint8_t interval);
        ~ChannelState();

        void startSending();
        void stopSending();
        void resetChannel(bool channel);

        VSignal<bool, bool> send_channel_state;

    private:
        class Channel
        {
        public:
            explicit Channel(uint8_t waitTime);

            void stopChannel();
            void resetTimer();

        public:
            bool is_alive = false;
            VTimer timer_alive;
            uint8_t wait_time;
        };

    private:
        std::shared_ptr<Channel> _n_channel;
        std::shared_ptr<Channel> _r_channel;

        VTimer _state_sender;
        uint8_t _interval;
    };

}

}
#endif //CHANNEL_STATE_H