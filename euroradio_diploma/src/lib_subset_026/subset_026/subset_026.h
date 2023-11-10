#ifndef SUBSET_026_H
#define SUBSET_026_H

#include "vsignal.h"
#include "vsettings.h"
#include "verror.h"

#include "sl.h"
#include "to_bin.h"
#include "to_dto.h"
#include "mediator_interface.h"
#include "channel_state.h"
#include "subset_interface.h"
#include "connection_table.h"
#include "lag_controller.h"

namespace euroradio
{
namespace subset_026
{

class Subset_026 : public subset_026::IConnectionController
{
public:

    Subset_026( const vsettings &settings,
                MediatorInterface &interface );

//============================== СИГНАЛЫ ================================================
    VSignal<Sl::DisRepIndicationData,
    Ale::DisconnectIndicationData,
    uint32_t>  disconnect_indication;

    VSignal<uint16_t> connect_indication;
//================================= ДЕЙСТВИЯ ============================================
    bool connectRequest (int32_t  brus_id,
                         int32_t  number,
                         uint32_t etcsid);

    void connectResponse(uint16_t tcepid,
                         uint32_t time,
                         bool     ack,
                         int8_t   last_balise_id,
                         int8_t   version);

    void disconnectRequest(uint16_t tcepidxmt,
                           uint8_t  reason,
                           uint8_t  sub_reason) override;
    void dataRequest      (uint32_t brus_id, const std::string &message);

    void sendMessage(const subset_026::ValidatedTrainDataMsg  &msg);
    void sendMessage(const subset_026::MaRequestMsg           &msg);
    void sendMessage(const subset_026::TrainPositionReportMsg &msg);
    void sendMessage(const subset_026::AckMsg                 &msg);
    void sendMessage(const subset_026::AckOfESMsg             &msg);
    void sendMessage(const subset_026::EndOfMissionMsg        &msg);
    void sendMessage(const subset_026::NoCVSMsg               &msg);
    void sendMessage(const subset_026::InitiationCSMsg        &msg);
    void sendMessage(const subset_026::TerminationCSMsg       &msg);
    void sendMessage(const subset_026::StartOfMissionPrMsg    &msg);
    void sendMessage(const subset_026::SessionEstablishedMsg  &msg);
    void sendMessage(const subset_026::ActualDistanceMsg      &msg);

    void sendMessage(const uint32_t brus_id, const subset_026::SrAuthMsg              &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::MovementAuthorityMsg   &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::AckOfTrainDataMsg      &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::UnconditionalESMsg     &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::RevocationOfESMsg      &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::GeneralMessage         &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::SystemVersionMsg       &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::AckOfTerminationCSMsg  &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::TrainRejectedMsg       &msg);
    void sendMessage(const uint32_t brus_id, const subset_026::TrainAcceptedMsg       &msg);

    void sendChannelStateMsg(bool is_normal_alive, bool is_redundant_alive);

    bool checkTimestamp(int32_t rbc_id, uint32_t new_t_train);

    std::map<uint32_t, ConnectionTable::ConnParams> getParameters();

    uint32_t    getTimestamp() const;
    uint32_t    getRbcId    () const;
    void        setNoCsInit (bool state);
    void        breakSession();

    std::shared_ptr<ConnectionTable> _connection_table;

    void disconnectByPeer( uint32_t peer_etcsid,
                           uint8_t  reason,
                           uint8_t  sub_reason  ) override;
//=======================================================================================
private:
    void parseAndSend             (const Sl::DataIndicationData &msg_data);
    void loadSettings             (const vsettings              &settings);
//============================ ДЕЙСТВИЯ ПО СИГНАЛАМ =====================================
    void onConnectIndication      (uint16_t tcepid);
    void onConnectConfirmation    (uint16_t tcepid);
    void onDisconnectIndication   (const Sl::DisRepIndicationData      &sl_data,
                                   const Ale::DisconnectIndicationData &ale_data);
    void onDataIndication         (const Sl::DataIndicationData        &msg_data);
    void onALEConnectIndication   (uint16_t tcepid, uint32_t etcsid);
    void onALEConnectConfirmation (uint16_t tcepid, uint32_t etcsid);
//=======================================================================================
private:
    uint32_t            _rbc_id;
    std::shared_ptr<Sl> _sl;
    MediatorInterface   &_interface;
    VSystemTimePoint    _start_time;
    int                 _allowed_diff = 0;

    std::shared_ptr<ChannelState>  _channel_state;

    bool _lag_control_on = false;
    std::shared_ptr<LagController> _lag_controller;
};

}
}

#endif //SUBSET_026_H