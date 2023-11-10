#ifndef RBC_GW_TOBIN_H
#define RBC_GW_TOBIN_H

#include <iostream>
#include <algorithm>
#include <chrono>
//=======================================================================================
#include "binary_message.h"
#include "messages.h"
#include "constants.h"
//=======================================================================================
/**
 * Binary message builder
 */
namespace euroradio
{

namespace subset_026
{

namespace parsers
{

namespace to_bin
{
//======================================== MESSAGES =====================================
/*!
 * Forms subset_026 header
 * @param msg Message to write to
 * @param id Message ID
 * @param length Message length
 * @param time Timestamp
 * @param etcs Device id
 */
//========================================= HEADERS =====================================
size_t formBrusHeader(BinaryMessage &msg, MessageID id, size_t length, const subset_026::BrusHeader &header);
size_t formRbcHeader(BinaryMessage &msg, MessageID id, size_t length, const subset_026::RbcHeader &header);
//===================================== RBC -> BRUS =====================================
BinaryMessage formSrAuthMsg(const subset_026::SrAuthMsg &msg);
BinaryMessage formMovementAuthorityMsg(const subset_026::MovementAuthorityMsg &msg);
BinaryMessage formAckOfTrainDataMsg(const subset_026::AckOfTrainDataMsg &msg);
BinaryMessage formUnconditionalESMsg(const subset_026::UnconditionalESMsg &msg);
BinaryMessage formRevocationOfESMsg(const subset_026::RevocationOfESMsg &msg);
BinaryMessage formGeneralMessage(const subset_026::GeneralMessage &msg);
BinaryMessage formSystemVersionMsg(const subset_026::SystemVersionMsg &msg);
BinaryMessage formAckOfTerminationCSMsg(const subset_026::AckOfTerminationCSMsg &msg);
BinaryMessage formTrainRejectedMsg(const subset_026::TrainRejectedMsg &msg);
BinaryMessage formTrainAcceptedMsg(const subset_026::TrainAcceptedMsg &msg);
//===================================== BRUS -> RBC =====================================
BinaryMessage formValidatedTrainDataMsg(const subset_026::ValidatedTrainDataMsg &msg);
BinaryMessage formMaRequestMsg(const subset_026::MaRequestMsg &msg);
BinaryMessage formTrainPositionReportMsg(const subset_026::TrainPositionReportMsg &msg);
BinaryMessage formAckMsg(const subset_026::AckMsg &msg);
BinaryMessage formAckOfESMsg(const subset_026::AckOfESMsg &msg);
BinaryMessage formEndOfMissionMsg(const subset_026::EndOfMissionMsg &msg);
BinaryMessage formNoCVSMsg(const subset_026::NoCVSMsg &msg);
BinaryMessage formInitiationCSMsg(const subset_026::InitiationCSMsg &msg);
BinaryMessage formTerminationCSMsg(const subset_026::TerminationCSMsg &msg);
BinaryMessage formStartOfMissionPrMsg(const subset_026::StartOfMissionPrMsg &msg);
BinaryMessage formSessionEstablishedMsg(const subset_026::SessionEstablishedMsg &msg);
BinaryMessage formChannelStateMsg(const subset_026::ChannelStateMsg &msg);
BinaryMessage formActualDistanceMsg(const subset_026::ActualDistanceMsg &msg);
//======================================== PACKAGES =====================================
size_t formBrusPkgHeader(BinaryMessage &msg, PackageID id, size_t length);
size_t formRbcPkgHeader(BinaryMessage &msg, PackageID id, size_t length);
//===================================== BRUS -> RBC =====================================
size_t formPositionReportPkg(BinaryMessage &msg, const subset_026::PositionReportPkg &pkg);
size_t formTrainRunningNumberPkg(BinaryMessage &msg, const subset_026::TrainRunningNumberPkg &number);
size_t countVtdPkgSize(const subset_026::ValidatedTrainDataPkg &data);
size_t formValidatedTrainDataPkg(BinaryMessage &msg, const subset_026::ValidatedTrainDataPkg &data);
size_t formAppDataPkg(BinaryMessage &msg, const subset_026::AppDataFromBrusPkg &pkg);
//===================================== RBC -> BRUS =====================================
size_t countLinkingPkgSize(const subset_026::LinkingPkg &data);
size_t formLinkingPkg(BinaryMessage &msg, const subset_026::LinkingPkg &pkg);
size_t countMaPkgSize(const subset_026::MovementAuthPkg &data);
size_t formMovementAuthPkg(BinaryMessage &msg, const subset_026::MovementAuthPkg &pkg);
size_t countLtOrderPkgSize(const subset_026::LvlTransitionOrderPkg &data);
size_t formLvlTransitionOrderPkg(BinaryMessage &msg, const subset_026::LvlTransitionOrderPkg &pkg);
size_t formSessionManagementPkg(BinaryMessage &msg, const subset_026::SessionManagementPkg &pkg);
size_t formAppDataPkg(BinaryMessage &msg, const subset_026::AppDataFromRbcPkg &pkg);
size_t countListOfBalisesPkgSize(const subset_026::ListOfBalisesInSrAuthPkg &data);
size_t formListOfBalisesInSrAuthPkg(BinaryMessage &msg, const subset_026::ListOfBalisesInSrAuthPkg &pkg);
size_t formTemporarySpeedRestrPkg(BinaryMessage &msg, const subset_026::TemporarySpeedRestrPkg &pkg);
size_t formTemporarySpeedRestrRevPkg(BinaryMessage &msg, const subset_026::TemporarySpeedRestrRevPkg &pkg);
//=======================================================================================
size_t toByte(const MessageLength &length);
size_t toByte(size_t lenght_in_bits);
}

}

}

}

#endif //RBC_GW_TOBIN_H