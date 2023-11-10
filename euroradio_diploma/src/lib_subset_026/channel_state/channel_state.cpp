#include "channel_state.h"

namespace euroradio
{

namespace subset_026
{

ChannelState::Channel::Channel(uint8_t waitTime)
: wait_time(waitTime)
{}

void ChannelState::Channel::stopChannel()
{
    is_alive = false;
    timer_alive.stop();
}

void ChannelState::Channel::resetTimer()
{
    timer_alive.stop();
    timer_alive.start(std::chrono::seconds(wait_time));
}

ChannelState::ChannelState(uint8_t wait_time, uint8_t interval)
: _interval(interval),
  _n_channel(std::make_shared<Channel>(wait_time)),
  _r_channel(std::make_shared<Channel>(wait_time))
{}

void ChannelState::startSending()
{
    _n_channel->timer_alive.timeout = [&]()
    {
        vrunlog << "ChannelState: Normal channel is out!";
        _n_channel->stopChannel();
    };

    _r_channel->timer_alive.timeout = [&]()
    {
        vrunlog << "ChannelState: Redundant channel is out!";
        _r_channel->stopChannel();
    };

    _state_sender.timeout = [&]()
    {
        send_channel_state(_n_channel->is_alive, _r_channel->is_alive);
    };

    _state_sender.start(std::chrono::seconds(_interval));
}

void ChannelState::resetChannel(bool channel)
{
    auto ch = (channel) ? _n_channel : _r_channel;
    ch->is_alive = true;
    ch->resetTimer();
}

void ChannelState::stopSending()
{
    _n_channel->stopChannel();
    _r_channel->stopChannel();

    _n_channel->timer_alive.timeout.unlink();
    _r_channel->timer_alive.timeout.unlink();

    _state_sender.stop();
    _state_sender.timeout.unlink();
}

ChannelState::~ChannelState()
{
    stopSending();
}

}

}