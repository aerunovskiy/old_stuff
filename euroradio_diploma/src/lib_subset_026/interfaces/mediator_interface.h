#ifndef RBC_GW_MEDIATORINTERFACE_H
#define RBC_GW_MEDIATORINTERFACE_H

#include "messages.h"

namespace euroradio
{

namespace subset_026
{

class MediatorInterface
{
public:
    virtual ~MediatorInterface();

    // On zcm receive
    virtual void dataMsg(const subset_026::ValidatedTrainDataMsg &msg) = 0;
    virtual void dataMsg(const subset_026::MaRequestMsg &msg) = 0;
    virtual void dataMsg(const subset_026::TrainPositionReportMsg &msg) = 0;
    virtual void dataMsg(const subset_026::AckMsg &msg) = 0;
    virtual void dataMsg(const subset_026::AckOfESMsg &msg) = 0;
    virtual void dataMsg(const subset_026::EndOfMissionMsg &msg) = 0;
    virtual void dataMsg(const subset_026::NoCVSMsg &msg) = 0;
    virtual void dataMsg(const subset_026::TerminationCSMsg &msg) = 0;
    virtual void dataMsg(const subset_026::SessionEstablishedMsg &msg) = 0;
    virtual void dataMsg(const subset_026::InitiationCSMsg &msg) = 0;
    virtual void dataMsg(const subset_026::StartOfMissionPrMsg &msg) = 0;
    virtual void dataMsg(const subset_026::ChannelStateMsg &msg) = 0;
    virtual void dataMsg(const subset_026::ActualDistanceMsg &msg) = 0;

    // On dto receive
    virtual void dtoMsg(const subset_026::ValidatedTrainDataMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::MaRequestMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::TrainPositionReportMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::AckMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::AckOfESMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::EndOfMissionMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::NoCVSMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::TerminationCSMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::SessionEstablishedMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::InitiationCSMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::StartOfMissionPrMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::ChannelStateMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::ActualDistanceMsg &msg) = 0;

    virtual void dtoMsg(const subset_026::SrAuthMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::MovementAuthorityMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::AckOfTrainDataMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::UnconditionalESMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::RevocationOfESMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::GeneralMessage &msg) = 0;
    virtual void dtoMsg(const subset_026::SystemVersionMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::AckOfTerminationCSMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::TrainRejectedMsg &msg) = 0;
    virtual void dtoMsg(const subset_026::TrainAcceptedMsg &msg) = 0;
};

}

}

#endif //RBC_GW_MEDIATORINTERFACE_H