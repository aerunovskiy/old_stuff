#include "to_bin.h"

namespace euroradio
{

namespace subset_026
{

namespace parsers
{
//======================================= MESSAGES ======================================
//======================================= HEADERS =======================================
size_t to_bin::formBrusHeader(BinaryMessage &msg,
                              MessageID id,
                              size_t length,
                              const subset_026::BrusHeader &header)
{

    msg.Append(size_t(id), VariableLength::NID_MESSAGE);
    msg.Append(length, VariableLength::L_MESSAGE);
    msg.Append(header.time, VariableLength::T_TRAIN);
    msg.Append(header.brus_id, VariableLength::NID_ENGINE);

    return static_cast<size_t>(MessageLength::BRUS_HEADER);
}
//=======================================================================================
size_t to_bin::formRbcHeader(BinaryMessage &msg,
                             MessageID id,
                             size_t length,
                             const subset_026::RbcHeader &header)
{
    msg.Append(size_t(id), VariableLength::NID_MESSAGE);
    msg.Append(length, VariableLength::L_MESSAGE);
    msg.Append(header.time, VariableLength::T_TRAIN);
    msg.Append(header.ack, VariableLength::M_ACK);
    msg.Append(header.last_balise_id, VariableLength::NID_LRBG);

    return static_cast<size_t>(MessageLength::RBC_HEADER);
}
//===================================== RBC -> BRUS =====================================
BinaryMessage to_bin::formSrAuthMsg(const subset_026::SrAuthMsg &msg)
{
    auto full_message_size = static_cast<size_t>(MessageLength::SR_AUTHORISATION);

    if (msg.balises.use_count() != 0)
        full_message_size += countListOfBalisesPkgSize(*msg.balises);

    BinaryMessage bin(toByte(full_message_size));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::SR_AUTHORISATION, toByte(full_message_size), msg.header);
    bit += bin.Append(msg.scale, VariableLength::Q_SCALE);
    bit += bin.Append(msg.distance, VariableLength::D_SR);

    if (msg.balises.use_count() != 0)
        bit += formListOfBalisesInSrAuthPkg(bin, *msg.balises);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formMovementAuthorityMsg(const subset_026::MovementAuthorityMsg &msg)
{
    auto full_message_size = static_cast<size_t>(MessageLength::MOVEMENT_AUTHORITY);

    full_message_size += countLinkingPkgSize(msg.links);
    full_message_size += countMaPkgSize(msg.movement_auth);

    if (msg.lvl_transition_order.use_count() != 0)
        full_message_size += countLtOrderPkgSize(*msg.lvl_transition_order);

    if (msg.speed_restr.use_count() != 0)
        full_message_size += static_cast<size_t>(PackageLength::SPEED_RESTR);

    if (msg.speed_restr_rev.use_count() != 0)
        full_message_size += static_cast<size_t>(PackageLength::SPEED_RESTR_REV);

    BinaryMessage bin(toByte(full_message_size));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::MOVEMENT_AUTHORITY, toByte(full_message_size), msg.header);
    bit += formLinkingPkg(bin, msg.links);
    bit += formMovementAuthPkg(bin, msg.movement_auth);

    if (msg.lvl_transition_order.use_count() != 0)
        bit += formLvlTransitionOrderPkg(bin, *msg.lvl_transition_order);

    if (msg.speed_restr.use_count() != 0)
        bit += formTemporarySpeedRestrPkg(bin, *msg.speed_restr);

    if (msg.speed_restr_rev.use_count() != 0)
        bit += formTemporarySpeedRestrRevPkg(bin, *msg.speed_restr_rev);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formAckOfTrainDataMsg(const subset_026::AckOfTrainDataMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ACK_TRAIN_DATA));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::ACK_TRAIN_DATA, toByte(MessageLength::ACK_TRAIN_DATA), msg.header);
    bit += bin.Append(msg.time, VariableLength::T_TRAIN);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formUnconditionalESMsg(const subset_026::UnconditionalESMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::UNCONDITIONAL_ES));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::UNCONDITIONAL_ES, toByte(MessageLength::UNCONDITIONAL_ES), msg.header);
    bit += bin.Append(msg.es_id, VariableLength::NID_EM);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formRevocationOfESMsg(const subset_026::RevocationOfESMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::REVOCATION_OF_ES));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::REVOCATION_OF_ES, toByte(MessageLength::REVOCATION_OF_ES), msg.header);
    bit += bin.Append(msg.es_id, VariableLength::NID_EM);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formGeneralMessage(const subset_026::GeneralMessage &msg)
{
    auto full_message_length = static_cast<size_t>(MessageLength::GENERAL_MSG);

    if (msg.session_management.use_count() != 0)
        full_message_length += static_cast<size_t>(PackageLength::SESSION_MNG);

    if (msg.app_data.use_count() != 0)
        full_message_length += static_cast<size_t>(PackageLength::RBC_APP_DATA);

    BinaryMessage bin(toByte(full_message_length));

    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::GENERAL_MSG, toByte(full_message_length), msg.header);

    if (msg.session_management.use_count() != 0)
        bit += formSessionManagementPkg(bin, *msg.session_management);

    if (msg.app_data.use_count() != 0)
        bit += formAppDataPkg(bin, *msg.app_data);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formSystemVersionMsg(const subset_026::SystemVersionMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::SYS_VERSION));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::SYS_VERSION, toByte(MessageLength::SYS_VERSION), msg.header);
    bit += bin.Append(msg.version, VariableLength::M_VERSION);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formAckOfTerminationCSMsg(const subset_026::AckOfTerminationCSMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ACK_TERMINATION));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::ACK_TERMINATION, toByte(MessageLength::ACK_TERMINATION), msg.header);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formTrainRejectedMsg(const subset_026::TrainRejectedMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::REJECTED));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::REJECTED, toByte(MessageLength::REJECTED), msg.header);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formTrainAcceptedMsg(const subset_026::TrainAcceptedMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ACCEPTED));
    size_t bit = 0;

    bit += formRbcHeader(bin, MessageID::ACCEPTED, toByte(MessageLength::ACCEPTED), msg.header);

    return bin;
}
//=======================================================================================
//===================================== BRUS -> RBC =====================================
BinaryMessage to_bin::formValidatedTrainDataMsg(const subset_026::ValidatedTrainDataMsg &msg)
{
    auto full_message_length = static_cast<size_t>(MessageLength::VALIDATED_TRAIN_DATA);

    full_message_length += countVtdPkgSize(msg.data);

    BinaryMessage bin(toByte(full_message_length));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::VALIDATED_TRAIN_DATA, toByte(full_message_length), msg.header);

    bit += formPositionReportPkg(bin, msg.position);
    bit += formTrainRunningNumberPkg(bin, msg.number);
    bit += formValidatedTrainDataPkg(bin, msg.data);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formMaRequestMsg(const subset_026::MaRequestMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::MA_REQUEST));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::MA_REQUEST, toByte(MessageLength::MA_REQUEST), msg.header);
    bit += bin.Append(msg.reason, VariableLength::Q_MARQSTREASON);
    bit += formPositionReportPkg(bin, msg.position);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formTrainPositionReportMsg(const subset_026::TrainPositionReportMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::TRAIN_POSITION_REPORT));
    size_t bit = 0;

    bit +=
        formBrusHeader(bin, MessageID::TRAIN_POSITION_REPORT, toByte(MessageLength::TRAIN_POSITION_REPORT), msg.header);
    bit += formPositionReportPkg(bin, msg.position);
    bit += formTrainRunningNumberPkg(bin, msg.number);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formAckMsg(const subset_026::AckMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ACKNOWLEDGEMENT));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::ACKNOWLEDGEMENT, toByte(MessageLength::ACKNOWLEDGEMENT), msg.header);

    bit += bin.Append(msg.confirmation_time, VariableLength::T_TRAIN);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formAckOfESMsg(const subset_026::AckOfESMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ES_ACKNOWLEDGEMENT));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::ES_ACKNOWLEDGEMENT, toByte(MessageLength::ES_ACKNOWLEDGEMENT), msg.header);
    bit += bin.Append(msg.es_id, VariableLength::NID_EM);
    bit += bin.Append(msg.is_stop, VariableLength::Q_EMERGENCYSTOP);
    bit += formPositionReportPkg(bin, msg.position);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formEndOfMissionMsg(const subset_026::EndOfMissionMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::END_OF_MISSION));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::END_OF_MISSION, toByte(MessageLength::END_OF_MISSION), msg.header);
    bit += formPositionReportPkg(bin, msg.position);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formNoCVSMsg(const subset_026::NoCVSMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::NO_COMPATIBLE_VERSION));

    formBrusHeader(bin, MessageID::NO_COMPATIBLE_VERSION, toByte(MessageLength::NO_COMPATIBLE_VERSION), msg.header);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formInitiationCSMsg(const subset_026::InitiationCSMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::CS_INITIATION));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::CS_INITIATION, toByte(MessageLength::CS_INITIATION), msg.header);
    bit += formTrainRunningNumberPkg(bin, msg.number);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formTerminationCSMsg(const subset_026::TerminationCSMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::CS_TERMINATION));

    formBrusHeader(bin, MessageID::CS_TERMINATION, toByte(MessageLength::CS_TERMINATION), msg.header);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formStartOfMissionPrMsg(const subset_026::StartOfMissionPrMsg &msg)
{
    auto full_message_size = static_cast<size_t>(MessageLength::START_OF_MISSION_PR);

    if (msg.app_data.use_count() != 0)
        full_message_size += static_cast<size_t>(PackageLength::BRUS_APP_DATA);

    BinaryMessage bin(toByte(full_message_size));
    size_t bit = 0;

    bit += formBrusHeader(bin, MessageID::START_OF_MISSION_PR, toByte(full_message_size), msg.header);
    bit += bin.Append(msg.status, VariableLength::Q_STATUS);
    bit += formPositionReportPkg(bin, msg.position);
    bit += formTrainRunningNumberPkg(bin, msg.number);

    if (msg.app_data.use_count() != 0)
        bit += formAppDataPkg(bin, *msg.app_data);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formSessionEstablishedMsg(const subset_026::SessionEstablishedMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::CS_ESTABLISHED));

    formBrusHeader(bin, MessageID::CS_ESTABLISHED, toByte(MessageLength::CS_ESTABLISHED), msg.header);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formChannelStateMsg(const subset_026::ChannelStateMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::CHANNEL_STATE));
    size_t bit = 0;
    bit += formBrusHeader(bin, MessageID::CHANNEL_STATE, toByte(MessageLength::CHANNEL_STATE), msg.header);
    bit += bin.Append(msg.is_normal_alive, ChannelStateVarLength::Q_NORMAL);
    bit += bin.Append(msg.is_redundant_alive, ChannelStateVarLength::Q_REDUNDANT);

    return bin;
}
//=======================================================================================
BinaryMessage to_bin::formActualDistanceMsg(const subset_026::ActualDistanceMsg &msg)
{
    BinaryMessage bin(toByte(MessageLength::ACTUAL_DISTANCE));
    size_t bit = 0;
    bit += formBrusHeader(bin, MessageID::ACTUAL_DISTANCE, toByte(MessageLength::ACTUAL_DISTANCE), msg.header);
    bit += bin.Append(msg.distance, VariableLength::D_ACTUAL);
    return bin;
}
//======================================== PACKAGES =====================================
//======================================= RBC -> BRUS ===================================
size_t to_bin::formRbcPkgHeader(BinaryMessage &msg, PackageID id, size_t length)
{
    msg.Append(static_cast<size_t>(id), VariableLength::NID_PACKET);
    msg.Append(static_cast<uint8_t>(1), VariableLength::Q_DIR); //TODO убрать хардкод
    msg.Append(static_cast<size_t>(length), VariableLength::L_PACKET);

    return static_cast<size_t>(PackageLength::RBC_HEADER);
}
//=======================================================================================
size_t to_bin::countLinkingPkgSize(const subset_026::LinkingPkg &data)
{
    auto result = static_cast<size_t>( PackageLength::LINKING);

    auto it_length = VariableLength::D_LINK +
        VariableLength::Q_NEWCOUNTRY +
        VariableLength::NID_C +
        VariableLength::NID_BG +
        VariableLength::Q_LINKORIENTATION +
        VariableLength::Q_LINKREACTION +
        VariableLength::Q_LOCACC;

    result += it_length * data.balises.size();

    return result;
}
//=======================================================================================
size_t to_bin::formLinkingPkg(BinaryMessage &msg, const subset_026::LinkingPkg &pkg)
{
    auto pkg_length = countLinkingPkgSize(pkg);

    formRbcPkgHeader(msg, PackageID::LINKING, pkg_length);

    msg.Append(pkg.scale, VariableLength::Q_SCALE);
    msg.Append(pkg.last.balise_dist, VariableLength::D_LINK);
    msg.Append(pkg.last.new_country, VariableLength::Q_NEWCOUNTRY);
    msg.Append(pkg.last.zone, VariableLength::NID_C);
    msg.Append(pkg.last.balise_group_id, VariableLength::NID_BG);
    msg.Append(pkg.last.balise_group_orientation, VariableLength::Q_LINKORIENTATION);
    msg.Append(pkg.last.on_link_error, VariableLength::Q_LINKREACTION);
    msg.Append(pkg.last.location_accuracy, VariableLength::Q_LOCACC);

    if (pkg.balises.size() <= 30)
        msg.Append(pkg.balises.size(), VariableLength::N_ITER);
    else
        msg.Append(30, VariableLength::N_ITER);

    for (size_t i = 0; (i < pkg.balises.size()) && (i < 30); ++i) {
        msg.Append(pkg.balises[i].balise_dist, VariableLength::D_LINK);
        msg.Append(pkg.balises[i].new_country, VariableLength::Q_NEWCOUNTRY);
        msg.Append(pkg.balises[i].zone, VariableLength::NID_C);
        msg.Append(pkg.balises[i].balise_group_id, VariableLength::NID_BG);
        msg.Append(pkg.balises[i].balise_group_orientation, VariableLength::Q_LINKORIENTATION);
        msg.Append(pkg.balises[i].on_link_error, VariableLength::Q_LINKREACTION);
        msg.Append(pkg.balises[i].location_accuracy, VariableLength::Q_LOCACC);
    }

    return pkg_length;
}
//=======================================================================================
size_t to_bin::countMaPkgSize(const subset_026::MovementAuthPkg &data)
{
    auto result = static_cast<size_t>(PackageLength::MOVEMENT_AUTH);

    auto it_length = VariableLength::L_SECTION +
        VariableLength::Q_SECTIONTIMER +
        VariableLength::T_SECTIONTIMER +
        VariableLength::D_SECTIONTIMERSTOPLOC;

    result += it_length * data.sections.size();

    return result;
}
//=======================================================================================
size_t to_bin::formMovementAuthPkg(BinaryMessage &msg, const subset_026::MovementAuthPkg &pkg)
{
    auto pkg_length = countMaPkgSize(pkg);

    formRbcPkgHeader(msg, PackageID::MOVEMENT_AUTH, pkg_length);

    msg.Append(pkg.scale, VariableLength::Q_SCALE);
    msg.Append(pkg.allowed_speed_end, VariableLength::V_EMA);
    msg.Append(pkg.speed_expire_time, VariableLength::T_EMA);

    msg.Append(pkg.sections.size(), VariableLength::N_ITER);

    for (const auto &s : pkg.sections) {
        msg.Append(s.length, VariableLength::L_SECTION);
        msg.Append(s.timer, VariableLength::Q_SECTIONTIMER);
        msg.Append(s.zone_expiration_time, VariableLength::T_SECTIONTIMER);
        msg.Append(s.length_to_stop, VariableLength::D_SECTIONTIMERSTOPLOC);
    }

    msg.Append(pkg.last.length, VariableLength::L_ENDSECTION);
    msg.Append(pkg.last.timer, VariableLength::Q_SECTIONTIMER);
    msg.Append(pkg.last.zone_expiration_time, VariableLength::T_SECTIONTIMER);
    msg.Append(pkg.last.length_to_stop, VariableLength::D_SECTIONTIMERSTOPLOC);

    msg.Append(pkg.is_timer, VariableLength::Q_ENDTIMER);
    msg.Append(pkg.zone_expiration_time, VariableLength::T_ENDTIMER);
    msg.Append(pkg.timer_start_distance, VariableLength::D_ENDTIMERSTARTLOC);
    msg.Append(pkg.is_danger_point, VariableLength::Q_DANGERPOINT);
    msg.Append(pkg.distance_from_auth, VariableLength::D_DP);
    msg.Append(pkg.danger_speed, VariableLength::V_RELEASEDP);

    msg.Append(pkg.is_overlap, VariableLength::Q_OVERLAP);
    msg.Append(pkg.distance_from_start, VariableLength::D_STARTOL);
    msg.Append(pkg.overlap_time, VariableLength::T_OL);
    msg.Append(pkg.distance_from_end, VariableLength::D_OL);
    msg.Append(pkg.overlap_speed, VariableLength::V_RELEASEOL);

    return pkg_length;
}
//=======================================================================================
size_t to_bin::countLtOrderPkgSize(const subset_026::LvlTransitionOrderPkg &data)
{
    auto result = static_cast<size_t>(PackageLength::LVL_TRANSITION_ORDER);

    auto it_length = VariableLength::M_LEVELTR +
        VariableLength::NID_NTC +
        VariableLength::L_ACKLEVELTR;

    result += it_length * data.orders.size();

    return result;
}
//=======================================================================================
size_t to_bin::formLvlTransitionOrderPkg(BinaryMessage &msg, const subset_026::LvlTransitionOrderPkg &pkg)
{
    auto pkg_length = countLtOrderPkgSize(pkg);

    formRbcPkgHeader(msg, PackageID::LVL_TRANSITION_ORDER, pkg_length);

    msg.Append(pkg.scale, VariableLength::Q_SCALE);
    msg.Append(pkg.change_distance, VariableLength::D_LEVELTR);
    msg.Append(pkg.current.order_lvl, VariableLength::M_LEVELTR);
    msg.Append(pkg.current.nid, VariableLength::NID_NTC);
    msg.Append(pkg.current.ack_distance, VariableLength::L_ACKLEVELTR);

    msg.Append(pkg.orders.size(), VariableLength::N_ITER);

    for (const auto &o : pkg.orders) {
        /*
         * Crutch for "big" order level
         */
        if (o.order_lvl == 13)
            msg.Append(3, VariableLength::M_LEVELTR);
        else
            msg.Append(o.order_lvl, VariableLength::M_LEVELTR);

        msg.Append(o.nid, VariableLength::NID_NTC);
        msg.Append(o.ack_distance, VariableLength::L_ACKLEVELTR);
    }

    return pkg_length;
}
//=======================================================================================
size_t to_bin::formSessionManagementPkg(BinaryMessage &msg, const subset_026::SessionManagementPkg &pkg)
{
    formRbcPkgHeader(msg, PackageID::SESSION_MNG, static_cast<size_t>(PackageLength::SESSION_MNG));

    msg.Append(pkg.control_flag, VariableLength::Q_RBC);
    msg.Append(pkg.zone_id, VariableLength::NID_C);
    msg.Append(pkg.rbc_id, VariableLength::NID_RBC);
    msg.Append(pkg.rbc_num, VariableLength::NID_RADIO);
    msg.Append(pkg.is_alive, VariableLength::Q_SLEEPSESSION);

    return static_cast<size_t>(PackageLength::SESSION_MNG);
}
//=======================================================================================
size_t to_bin::formAppDataPkg(BinaryMessage &msg, const subset_026::AppDataFromRbcPkg &pkg)
{
    formRbcPkgHeader(msg, PackageID::RBC_APP_DATA, static_cast<size_t>(PackageLength::RBC_APP_DATA));

    msg.Append(pkg.system_id, VariableLength::NID_XUSER);
    msg.Append(pkg.zone, AppDataVarLength::ID_AREA);
    msg.Append(pkg.balise_id, AppDataVarLength::NID_BALISE);
    msg.Append(pkg.distance, AppDataVarLength::DISTANCE);
    msg.Append(pkg.crc_32, AppDataVarLength::CRC_32);

    return static_cast<size_t>(PackageLength::RBC_APP_DATA);
}
//=======================================================================================
size_t to_bin::countListOfBalisesPkgSize(const subset_026::ListOfBalisesInSrAuthPkg &data)
{
    auto result = static_cast<size_t>(PackageLength::LIST_OF_BALISES);

    auto it_length = VariableLength::Q_NEWCOUNTRY +
        VariableLength::NID_C +
        VariableLength::NID_BG;

    result += it_length * data.balises.size();

    return result;
}
//=======================================================================================
size_t to_bin::formListOfBalisesInSrAuthPkg(BinaryMessage &msg, const subset_026::ListOfBalisesInSrAuthPkg &pkg)
{
    auto pkg_length = countListOfBalisesPkgSize(pkg);

    formRbcPkgHeader(msg, PackageID::LIST_OF_BALISES, pkg_length);

    msg.Append(pkg.balises.size(), VariableLength::N_ITER);

    for (const auto &b : pkg.balises) {
        msg.Append(b.new_zone, VariableLength::Q_NEWCOUNTRY);
        msg.Append(b.zone_id, VariableLength::NID_C);
        msg.Append(b.balise_group_id, VariableLength::NID_BG);
    }

    return pkg_length;
}
//=======================================================================================
size_t to_bin::formTemporarySpeedRestrPkg(BinaryMessage &msg, const subset_026::TemporarySpeedRestrPkg &pkg)
{
    formRbcPkgHeader(msg, PackageID::SPEED_RESTR, static_cast<size_t>(PackageLength::SPEED_RESTR));

    msg.Append(pkg.scale, VariableLength::Q_SCALE);
    msg.Append(pkg.restr_id, VariableLength::NID_TSR);
    msg.Append(pkg.enable_dist, VariableLength::D_TSR);
    msg.Append(pkg.active_dist, VariableLength::L_TSR);
    msg.Append(pkg.is_valid, VariableLength::Q_FRONT);
    msg.Append(pkg.allowed_speed, VariableLength::V_TSR);

    return static_cast<size_t>(PackageLength::SPEED_RESTR);
}
//=======================================================================================
size_t to_bin::formTemporarySpeedRestrRevPkg(BinaryMessage &msg, const subset_026::TemporarySpeedRestrRevPkg &pkg)
{
    formRbcPkgHeader(msg, PackageID::SPEED_RESTR_REV, static_cast<size_t>(PackageLength::SPEED_RESTR_REV));

    msg.Append(pkg.restr_id, VariableLength::NID_TSR);

    return static_cast<size_t>(PackageLength::SPEED_RESTR_REV);
}
//=======================================================================================
//===================================== BRUS -> RBC =====================================
size_t to_bin::formBrusPkgHeader(BinaryMessage &msg, PackageID id, size_t length)
{
    msg.Append(static_cast<size_t>(id), VariableLength::NID_PACKET);
    msg.Append(length, VariableLength::L_PACKET);

    return static_cast<size_t>(PackageLength::BRUS_HEADER);
}
//=======================================================================================
size_t to_bin::formPositionReportPkg(BinaryMessage &msg, const subset_026::PositionReportPkg &pkg)
{
    formBrusPkgHeader(msg, PackageID::POSITION_REPORT, static_cast<size_t>(PackageLength::POSITION_REPORT));

    msg.Append(pkg.scale, VariableLength::Q_SCALE); // distance scale

    msg.Append(pkg.last_balise_id, VariableLength::NID_LRBG); // last passed balise id
    msg.Append(pkg.distance_passed, VariableLength::D_LRBG); // distance since last balise

    msg.Append(pkg.balise_orientation, VariableLength::Q_DIRLRBG); // train move direction
    msg.Append(pkg.balise_side, VariableLength::Q_DLRBG); // balise side

    msg.Append(pkg.upper_confid_int, VariableLength::L_DOUBTOVER); // upper confidential interval limit
    msg.Append(pkg.lower_confid_int, VariableLength::L_DOUBTUNDER); // lower confidential interval limit

    msg.Append(pkg.train_integrity, VariableLength::Q_LENGTH); // train integrity
    msg.Append(pkg.length_in_cars, VariableLength::L_TRAININT); // train length in cars
    msg.Append(pkg.speed, VariableLength::V_TRAIN); // train current speed
    msg.Append(pkg.is_nominal_direction, VariableLength::Q_DIRTRAIN); // balise relative train direction
    msg.Append(pkg.control_mode, VariableLength::M_MODE); // on board train control mode
    msg.Append(pkg.control_lvl, VariableLength::M_LEVEL); // control lvl //TODO idk what value to take
    msg.Append(pkg.nid, VariableLength::NID_NTC); // national system id

    return static_cast<size_t>(PackageLength::POSITION_REPORT);
}
//=======================================================================================
size_t to_bin::formTrainRunningNumberPkg(BinaryMessage &msg, const subset_026::TrainRunningNumberPkg &number)
{
    formBrusPkgHeader(msg, PackageID::TRAIN_RUNNING_NUMBER, static_cast<size_t>(PackageLength::TRAIN_RUNNING_NUMBER));

    msg.Append(number.number, VariableLength::NID_OPERATIONAL);

    return static_cast<size_t>(PackageLength::TRAIN_RUNNING_NUMBER);
}
//=======================================================================================
size_t to_bin::countVtdPkgSize(const subset_026::ValidatedTrainDataPkg &data)
{
    auto result = static_cast<size_t>(PackageLength::VALIDATED_TRAIN_DATA);

    result += (VariableLength::M_VOLTAGE + VariableLength::NID_CTRACTION) * data.axis.size();
    result += VariableLength::NID_NTC * data.nids.size();

    return result;
}
//=======================================================================================
size_t to_bin::formValidatedTrainDataPkg(BinaryMessage &msg, const subset_026::ValidatedTrainDataPkg &data)
{
    auto pkg_length = countVtdPkgSize(data);

    formBrusPkgHeader(msg, PackageID::VALIDATED_TRAIN_DATA, pkg_length);

    msg.Append(data.train_category, VariableLength::NC_CDTRAIN);     // Train category
    msg.Append(data.train_type, VariableLength::NC_TRAIN);       // Train category
    msg.Append(data.length_in_cars, VariableLength::L_TRAIN);        // Train length
    msg.Append(data.max_speed, VariableLength::V_MAXTRAIN);     // Max train speed (32 is hardcoded)
    msg.Append(data.loading_prof, VariableLength::M_LOADINGGAUGE); // Train gaugge profile (0 is hardcoded)
    msg.Append(data.axis_load, VariableLength::M_AXLELOADCAT);  // Axis load profile(2 is hardcoded idk why)
    msg.Append(data.is_airtight,
               VariableLength::M_AIRTIGHT);     // Availability of a tightness system (2 is hardcoded idk why)
    msg.Append(data.axis_number, VariableLength::N_AXLE);         // Availability of a tightness system
    msg.Append(data.axis.size(), VariableLength::N_ITER);

    for (const auto &a : data.axis) {
        msg.Append(a.voltage, VariableLength::M_VOLTAGE);
        msg.Append(a.nid, VariableLength::NID_CTRACTION);
    }

    msg.Append(data.nids.size(), VariableLength::N_ITER);

    for (const auto &n : data.nids)
        msg.Append(n, VariableLength::NID_NTC);

    return pkg_length;
}
//=======================================================================================
size_t to_bin::formAppDataPkg(BinaryMessage &msg, const subset_026::AppDataFromBrusPkg &pkg)
{
    formBrusPkgHeader(msg, PackageID::BRUS_APP_DATA, static_cast<size_t>(PackageLength::BRUS_APP_DATA));

    msg.Append(pkg.system_id, VariableLength::NID_XUSER);
    msg.Append(pkg.map_version, AppDataVarLength::MAP_VER);

    return static_cast<size_t>(PackageLength::BRUS_APP_DATA);
}
//=======================================================================================
size_t to_bin::toByte(const MessageLength &length)
{
    return static_cast<size_t>(ceil(static_cast<size_t>(length) / 8.));
}
//=======================================================================================
size_t to_bin::toByte(size_t lenght_in_bits)
{
    return static_cast<size_t>(ceil(lenght_in_bits / 8.));
}
//=======================================================================================
}
}
}