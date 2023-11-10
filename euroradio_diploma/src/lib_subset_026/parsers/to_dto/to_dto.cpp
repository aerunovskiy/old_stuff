//=======================================================================================
#include "to_dto.h"

namespace euroradio
{

namespace subset_026
{

namespace parsers
{
//======================================= MESSAGES ======================================
//======================================= HEADERS =======================================
subset_026::Header to_dto::parseHeader(const uint8_t *data, size_t &bit_iterator)
{
    return {get<uint8_t>(data, bit_iterator, VariableLength::NID_MESSAGE),
            get<uint16_t>(data, bit_iterator, VariableLength::L_MESSAGE),
            get<uint32_t>(data, bit_iterator, VariableLength::T_TRAIN)};
}
//=======================================================================================
subset_026::BrusHeader to_dto::parseBrusHeader(const uint8_t *data,
                                               size_t &bit_iterator,
                                               const subset_026::Header &header)
{
    auto brus_id = get<uint32_t>(data, bit_iterator, VariableLength::NID_ENGINE);
    return {header, brus_id};
}
//=======================================================================================
subset_026::RbcHeader to_dto::parseRbcHeader(const uint8_t *data,
                                             size_t &bit_iterator,
                                             const subset_026::Header &header)
{
    auto ack = get<bool>(data, bit_iterator, VariableLength::M_ACK);
    auto last_balise_id = get<uint32_t>(data, bit_iterator, VariableLength::NID_LRBG);
    return {header,
            ack,
            last_balise_id};
}
//===================================== RBC -> BRUS =====================================
subset_026::SrAuthMsg to_dto::parseSrAuthMsg(const uint8_t *data,
                                             size_t &bit_iterator,
                                             const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);
    auto distance = get<uint16_t>(data, bit_iterator, VariableLength::D_SR);

    std::shared_ptr<subset_026::ListOfBalisesInSrAuthPkg> list_of_balises = nullptr;

    size_t length_in_bits = header.length * 8;

    if ((bit_iterator < length_in_bits) &&
        (length_in_bits - bit_iterator) >= static_cast<size_t>(PackageLength::RBC_HEADER)) {
        list_of_balises
            .reset(new subset_026::ListOfBalisesInSrAuthPkg(parseListOfBalisesInSrAuthPkg(data, bit_iterator)));
    }

    return {rbc_header,
            scale,
            distance,
            list_of_balises};
}
//=======================================================================================
subset_026::MovementAuthorityMsg to_dto::parseMovementAuthorityMsg(const uint8_t *data,
                                                                   size_t &bit_iterator,
                                                                   const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto links = parseLinkingPkg(data, bit_iterator);
    auto movement_auth = parseMovementAuthPkg(data, bit_iterator);

    std::shared_ptr<subset_026::LvlTransitionOrderPkg> lvl_transition_order = nullptr;
    std::shared_ptr<subset_026::TemporarySpeedRestrPkg> speed_restr = nullptr;
    std::shared_ptr<subset_026::TemporarySpeedRestrRevPkg> speed_restr_rev = nullptr;

    size_t length_in_bits = header.length * 8;

    while ((bit_iterator < length_in_bits) &&
        (length_in_bits - bit_iterator) >= static_cast<size_t>(PackageLength::RBC_HEADER)) {
        auto pkg_header = parseRbcPkgHeader(data, bit_iterator);

        auto id = static_cast<PackageID>(pkg_header.pkg_id);

        switch (id) {
            case PackageID::LVL_TRANSITION_ORDER: {
                lvl_transition_order
                    .reset(new subset_026::LvlTransitionOrderPkg(parseLvlTransitionOrderPkg(data, bit_iterator)));
                break;
            }
            case PackageID::SPEED_RESTR: {
                speed_restr
                    .reset(new subset_026::TemporarySpeedRestrPkg(parseTemporarySpeedRestrPkg(data, bit_iterator)));
                break;
            }
            case PackageID::SPEED_RESTR_REV: {
                speed_restr_rev.reset(new subset_026::TemporarySpeedRestrRevPkg(parseTemporarySpeedRestrRevPkg(data,
                                                                                                               bit_iterator)));
                break;
            }
            default: {
                vwarning << "Wrong package id!";
            }
        }
    }

    return {rbc_header,
            links,
            movement_auth,
            lvl_transition_order,
            speed_restr,
            speed_restr_rev};
}
//=======================================================================================
subset_026::AckOfTrainDataMsg to_dto::parseAckOfTrainDataMsg(const uint8_t *data,
                                                             size_t &bit_iterator,
                                                             const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto atd_time = get<uint32_t>(data, bit_iterator, VariableLength::T_TRAIN);

    return {rbc_header,
            atd_time};
}
//=======================================================================================
subset_026::UnconditionalESMsg to_dto::parseUnconditionalESMsg(const uint8_t *data,
                                                               size_t &bit_iterator,
                                                               const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto es_id = get<uint8_t>(data, bit_iterator, VariableLength::NID_EM);

    return {rbc_header,
            es_id};
}
//=======================================================================================
subset_026::RevocationOfESMsg to_dto::parseRevocationOfESMsg(const uint8_t *data,
                                                             size_t &bit_iterator,
                                                             const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto es_id = get<uint8_t>(data, bit_iterator, VariableLength::NID_EM);

    return {rbc_header,
            es_id};
}
//=======================================================================================
subset_026::GeneralMessage to_dto::parseGeneralMessage(const uint8_t *data,
                                                       size_t &bit_iterator,
                                                       const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);

    std::shared_ptr<subset_026::SessionManagementPkg> session_management = nullptr;
    std::shared_ptr<subset_026::AppDataFromRbcPkg> app_data = nullptr;

    if (bit_iterator < (header.length * 8))
        session_management.reset(new subset_026::SessionManagementPkg(parseSessionManagementPkg(data, bit_iterator)));

    if (bit_iterator < (header.length * 8))
        app_data.reset(new subset_026::AppDataFromRbcPkg(parseAppDataFromRbcPkg(data, bit_iterator)));

    return {rbc_header,
            session_management,
            app_data};
}
//=======================================================================================
subset_026::SystemVersionMsg to_dto::parseSystemVersionMsg(const uint8_t *data,
                                                           size_t &bit_iterator,
                                                           const subset_026::Header &header)
{
    auto rbc_header = parseRbcHeader(data, bit_iterator, header);
    auto version = get<uint8_t>(data, bit_iterator, VariableLength::M_VERSION);

    return {rbc_header, version};
}
//=======================================================================================
subset_026::AckOfTerminationCSMsg to_dto::parseAckOfTerminationCSMsg(const uint8_t *data,
                                                                     size_t &bit_iterator,
                                                                     const subset_026::Header &header)
{
    return parseRbcHeader(data, bit_iterator, header);
}
//=======================================================================================
subset_026::TrainRejectedMsg to_dto::parseTrainRejectedMsg(const uint8_t *data,
                                                           size_t &bit_iterator,
                                                           const subset_026::Header &header)
{
    return parseRbcHeader(data, bit_iterator, header);
}
//=======================================================================================
subset_026::TrainAcceptedMsg to_dto::parseTrainAcceptedMsg(const uint8_t *data,
                                                           size_t &bit_iterator,
                                                           const subset_026::Header &header)
{
    return parseRbcHeader(data, bit_iterator, header);
}
//===================================== BRUS -> RBC =====================================
subset_026::ValidatedTrainDataMsg to_dto::parseValidatedTrainDataMsg(const uint8_t *data,
                                                                     size_t &bit_iterator,
                                                                     const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto position = parsePositionReportPkg(data, bit_iterator);
    auto number = parseTrainRunningNumberPkg(data, bit_iterator);
    auto vt_data = parseValidatedTrainDataPkg(data, bit_iterator);

    return {brus_header,
            position,
            number,
            vt_data};
}
//=======================================================================================
subset_026::MaRequestMsg to_dto::parseMaRequestMsg(const uint8_t *data,
                                                   size_t &bit_iterator,
                                                   const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto reason = get<uint8_t>(data, bit_iterator, VariableLength::Q_MARQSTREASON);
    auto position = parsePositionReportPkg(data, bit_iterator);

    return {brus_header,
            reason,
            position};
}
//=======================================================================================
subset_026::TrainPositionReportMsg to_dto::parseTrainPositionReportMsg(const uint8_t *data,
                                                                       size_t &bit_iterator,
                                                                       const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto position = parsePositionReportPkg(data, bit_iterator);
    auto number = parseTrainRunningNumberPkg(data, bit_iterator);

    return {brus_header,
            position,
            number};
}
//=======================================================================================
subset_026::AckMsg to_dto::parseAckMsg(const uint8_t *data,
                                       size_t &bit_iterator,
                                       const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto confirmation_time = get<uint32_t>(data, bit_iterator, VariableLength::T_TRAIN);

    return {brus_header, confirmation_time};
}
//=======================================================================================
subset_026::AckOfESMsg to_dto::parseAckOfESMsg(const uint8_t *data,
                                               size_t &bit_iterator,
                                               const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto es_id = get<uint8_t>(data, bit_iterator, VariableLength::NID_EM);
    auto is_stop = get<bool>(data, bit_iterator, VariableLength::Q_EMERGENCYSTOP);
    auto position = parsePositionReportPkg(data, bit_iterator);

    return {brus_header,
            es_id,
            is_stop,
            position};
}
//=======================================================================================
subset_026::EndOfMissionMsg to_dto::parseEndOfMissionMsg(const uint8_t *data,
                                                         size_t &bit_iterator,
                                                         const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto position = parsePositionReportPkg(data, bit_iterator);

    return {brus_header, position};
}
//=======================================================================================
subset_026::NoCVSMsg to_dto::parseNoCVSMsg(const uint8_t *data,
                                           size_t &bit_iterator,
                                           const subset_026::Header &header)
{
    return parseBrusHeader(data, bit_iterator, header);
}
//=======================================================================================
subset_026::InitiationCSMsg to_dto::parseInitiationCSMsg(const uint8_t *data,
                                                         size_t &bit_iterator,
                                                         const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto train_number = parseTrainRunningNumberPkg(data, bit_iterator);

    return {brus_header, train_number.number};
}
//=======================================================================================
subset_026::TerminationCSMsg to_dto::parseTerminationCSMsg(const uint8_t *data,
                                                           size_t &bit_iterator,
                                                           const subset_026::Header &header)
{
    return parseBrusHeader(data, bit_iterator, header);
}
//=======================================================================================
subset_026::StartOfMissionPrMsg to_dto::parseStartOfMissionPrMsg(const uint8_t *data,
                                                                 size_t &bit_iterator,
                                                                 const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto status = get<bool>(data, bit_iterator, VariableLength::Q_STATUS);
    auto position = parsePositionReportPkg(data, bit_iterator);
    auto number = parseTrainRunningNumberPkg(data, bit_iterator);

    std::shared_ptr<subset_026::AppDataFromBrusPkg> app_data = nullptr;

    size_t length_in_bits = header.length * 8;

    if ((bit_iterator < length_in_bits) &&
        (length_in_bits - bit_iterator) >= static_cast<size_t>(PackageLength::BRUS_HEADER)) {
        app_data.reset(new subset_026::AppDataFromBrusPkg(parseAppDataFromBrusPkg(data, bit_iterator)));
    }

    return {brus_header,
            status,
            position,
            number,
            app_data};
}
//=======================================================================================
subset_026::SessionEstablishedMsg to_dto::parseSessionEstablishedMsg(const uint8_t *data,
                                                                     size_t &bit_iterator,
                                                                     const subset_026::Header &header)
{
    return parseBrusHeader(data, bit_iterator, header);
}
//=======================================================================================
subset_026::ChannelStateMsg to_dto::parseChannelStateMsg(const uint8_t *data,
                                                         size_t &bit_iterator,
                                                         const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto is_normal_alive = get<bool>(data, bit_iterator, ChannelStateVarLength::Q_NORMAL);
    auto is_redundant_alive = get<bool>(data, bit_iterator, ChannelStateVarLength::Q_REDUNDANT);
    return {brus_header,
            is_normal_alive,
            is_redundant_alive};
}
//=======================================================================================
subset_026::ActualDistanceMsg to_dto::parseActualDistanceMsg(const uint8_t *data,
                                                             size_t &bit_iterator,
                                                             const subset_026::Header &header)
{
    auto brus_header = parseBrusHeader(data, bit_iterator, header);
    auto distance = get<uint16_t>(data, bit_iterator, VariableLength::D_ACTUAL);
    return {brus_header, distance};
}
//====================================== PACKAGES =====================================++
//===================================== RBC -> BRUS =====================================
to_dto::RbcPkgHeader to_dto::parseRbcPkgHeader(const uint8_t *data, size_t &bit_iterator)
{
    auto id = get<uint8_t>(data, bit_iterator, VariableLength::NID_PACKET);
    auto data_direction = get<uint8_t>(data, bit_iterator, VariableLength::Q_DIR);
    auto length = get<uint16_t>(data, bit_iterator, VariableLength::L_PACKET);

    return {id, data_direction, length};
}
//=======================================================================================
subset_026::LinkingPkg to_dto::parseLinkingPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseRbcPkgHeader(data, bit_iterator);

    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);

    subset_026::BaliseGroup last{};

    last.balise_dist = get<uint16_t>(data, bit_iterator, VariableLength::D_LINK);
    last.new_country = get<bool>(data, bit_iterator, VariableLength::Q_NEWCOUNTRY);
    last.zone = get<uint16_t>(data, bit_iterator, VariableLength::NID_C);
    last.balise_group_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_BG);
    last.balise_group_orientation = get<bool>(data, bit_iterator, VariableLength::Q_LINKORIENTATION);
    last.on_link_error = get<uint8_t>(data, bit_iterator, VariableLength::Q_LINKREACTION);
    last.location_accuracy = get<uint8_t>(data, bit_iterator, VariableLength::Q_LOCACC);

    auto iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);
    std::vector<subset_026::BaliseGroup> balises;

    for (int8_t i = 0; i < iter; ++i) {
        subset_026::BaliseGroup balise{};

        balise.balise_dist = get<uint16_t>(data, bit_iterator, VariableLength::D_LINK);
        balise.new_country = get<bool>(data, bit_iterator, VariableLength::Q_NEWCOUNTRY); //Skip new_country (constant)
        balise.zone = get<uint16_t>(data, bit_iterator, VariableLength::NID_C); //Skip cause NID_C is 0 constant
        balise.balise_group_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_BG);
        balise.balise_group_orientation = get<bool>(data, bit_iterator, VariableLength::Q_LINKORIENTATION);
        balise.on_link_error = get<uint8_t>(data, bit_iterator, VariableLength::Q_LINKREACTION);
        balise.location_accuracy = get<uint8_t>(data, bit_iterator, VariableLength::Q_LOCACC);

        balises.emplace_back(balise);
    }

    return {scale,
            last,
            balises};
}
//=======================================================================================
subset_026::MovementAuthPkg to_dto::parseMovementAuthPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseRbcPkgHeader(data, bit_iterator);

    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);
    auto allowed_speed_end = get<uint8_t>(data, bit_iterator, VariableLength::V_EMA);
    auto speed_expire_time = get<uint16_t>(data, bit_iterator, VariableLength::T_EMA);

    auto iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);

    std::vector<subset_026::Section> sections{};

    for (int8_t i = 0; i < iter; ++i) {
        subset_026::Section s{};

        s.length = get<uint16_t>(data, bit_iterator, VariableLength::L_SECTION);
        s.timer = get<bool>(data, bit_iterator, VariableLength::Q_SECTIONTIMER);

        s.zone_expiration_time = get<uint16_t>(data, bit_iterator, VariableLength::T_SECTIONTIMER);
        s.length_to_stop = get<uint16_t>(data, bit_iterator, VariableLength::D_SECTIONTIMERSTOPLOC);

        sections.emplace_back(s);
    }

    subset_026::Section last{};

    last.length = get<uint16_t>(data, bit_iterator, VariableLength::L_ENDSECTION);
    last.timer = get<bool>(data, bit_iterator, VariableLength::Q_SECTIONTIMER);

    last.zone_expiration_time = get<uint16_t>(data, bit_iterator, VariableLength::T_SECTIONTIMER);
    last.length_to_stop = get<uint16_t>(data, bit_iterator, VariableLength::D_SECTIONTIMERSTOPLOC);

    auto is_timer = get<bool>(data, bit_iterator, VariableLength::Q_ENDTIMER);
    auto zone_expiration_time = get<uint16_t>(data, bit_iterator, VariableLength::T_ENDTIMER);
    auto timer_start_distance = get<uint16_t>(data, bit_iterator, VariableLength::D_ENDTIMERSTARTLOC);

    auto is_danger_point = get<bool>(data, bit_iterator, VariableLength::Q_DANGERPOINT);
    auto distance_from_auth = get<uint16_t>(data, bit_iterator, VariableLength::D_DP);
    auto danger_speed = get<uint8_t>(data, bit_iterator, VariableLength::V_RELEASEDP);

    auto
        is_overlap = get<bool>(data, bit_iterator, VariableLength::Q_OVERLAP); //Skip overlap, cause it's hardcoded to 0
    auto distance_from_start = get<uint16_t>(data, bit_iterator, VariableLength::D_STARTOL);
    auto overlap_time = get<uint16_t>(data, bit_iterator, VariableLength::T_OL);
    auto distance_from_end = get<uint16_t>(data, bit_iterator, VariableLength::D_OL);
    auto overlap_speed = get<uint8_t>(data, bit_iterator, VariableLength::V_RELEASEOL);

    return {scale,
            allowed_speed_end,
            speed_expire_time,
            sections,
            last,
            is_timer,
            zone_expiration_time,
            timer_start_distance,
            is_danger_point,
            distance_from_auth,
            danger_speed,
            is_overlap,
            distance_from_start,
            overlap_time,
            distance_from_end,
            overlap_speed};
}
//=======================================================================================
subset_026::LvlTransitionOrderPkg to_dto::parseLvlTransitionOrderPkg(const uint8_t *data, size_t &bit_iterator)
{
    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);
    auto change_distance = get<uint16_t>(data, bit_iterator, VariableLength::D_LEVELTR);

    subset_026::Order current{};

    current.order_lvl = get<uint16_t>(data, bit_iterator, VariableLength::M_LEVELTR);
    current.nid = get<uint8_t>(data, bit_iterator, VariableLength::NID_NTC);
    current.ack_distance = get<uint16_t>(data, bit_iterator, VariableLength::L_ACKLEVELTR);

    auto iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);

    std::vector<subset_026::Order> orders;

    for (int8_t i = 0; i < iter; ++i) {
        subset_026::Order ord{};

        /*
         * Crutch for "big" order level TODO resolve
         */
        auto order_lvl = get<uint16_t>(data, bit_iterator, VariableLength::M_LEVELTR);

        if (order_lvl == 3)
            ord.order_lvl = 13;
        else
            ord.order_lvl = order_lvl;

        ord.nid = get<uint8_t>(data, bit_iterator, VariableLength::NID_NTC);
        ord.ack_distance = get<uint16_t>(data, bit_iterator, VariableLength::L_ACKLEVELTR);

        orders.emplace_back(ord);
    }

    return {scale,
            change_distance,
            current,
            orders};
}
//=======================================================================================
subset_026::SessionManagementPkg to_dto::parseSessionManagementPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseRbcPkgHeader(data, bit_iterator);

    auto control_flag = get<bool>(data, bit_iterator, VariableLength::Q_RBC);
    auto zone_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_C);
    auto rbc_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_RBC);
    auto rbc_num = get<uint64_t>(data, bit_iterator, VariableLength::NID_RADIO);
    auto is_alive = get<bool>(data, bit_iterator, VariableLength::Q_SLEEPSESSION);

    return {control_flag,
            zone_id,
            rbc_id,
            rbc_num,
            is_alive};
}
//=======================================================================================
subset_026::AppDataFromRbcPkg to_dto::parseAppDataFromRbcPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseRbcPkgHeader(data, bit_iterator);

    auto system_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_XUSER);
    auto zone = get<uint32_t>(data, bit_iterator, AppDataVarLength::ID_AREA);
    auto balise = get<uint16_t>(data, bit_iterator, AppDataVarLength::NID_BALISE);
    auto distance = get<uint16_t>(data, bit_iterator, AppDataVarLength::DISTANCE);
    auto crc_32 = get<uint32_t>(data, bit_iterator, AppDataVarLength::CRC_32);

    return {system_id,
            zone,
            balise,
            distance,
            crc_32};
}
//=======================================================================================
subset_026::ListOfBalisesInSrAuthPkg to_dto::parseListOfBalisesInSrAuthPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseRbcPkgHeader(data, bit_iterator);

    auto iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);

    std::vector<subset_026::Balise> balises;

    for (int8_t i = 0; i < iter; ++i) {
        subset_026::Balise balise{};

        balise.new_zone = get<bool>(data, bit_iterator, VariableLength::Q_NEWCOUNTRY);
        balise.zone_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_C);
        balise.balise_group_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_BG);

        balises.emplace_back(balise);
    }

    return subset_026::ListOfBalisesInSrAuthPkg{balises};
}
//=======================================================================================
subset_026::TemporarySpeedRestrPkg to_dto::parseTemporarySpeedRestrPkg(const uint8_t *data, size_t &bit_iterator)
{
    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);
    auto restr_id = get<uint8_t>(data, bit_iterator, VariableLength::NID_TSR);
    auto enable_dist = get<uint16_t>(data, bit_iterator, VariableLength::D_TSR);
    auto active_dist = get<uint16_t>(data, bit_iterator, VariableLength::L_TSR);
    auto is_valid = get<bool>(data, bit_iterator, VariableLength::Q_FRONT);
    auto allowed_speed = get<uint8_t>(data, bit_iterator, VariableLength::V_TSR);

    return {scale,
            restr_id,
            enable_dist,
            active_dist,
            is_valid,
            allowed_speed};
}
//=======================================================================================
subset_026::TemporarySpeedRestrRevPkg to_dto::parseTemporarySpeedRestrRevPkg(const uint8_t *data, size_t &bit_iterator)
{
    auto restr_id = get<uint8_t>(data, bit_iterator, VariableLength::NID_TSR);

    return subset_026::TemporarySpeedRestrRevPkg{restr_id};
}
//===================================== BRUS -> RBC =====================================
to_dto::BrusPkgHeader to_dto::parseBrusPkgHeader(const uint8_t *data, size_t &bit_iterator)
{
    auto id = get<uint8_t>(data, bit_iterator, VariableLength::NID_PACKET);
    auto length = get<uint16_t>(data, bit_iterator, VariableLength::L_PACKET);

    return {id,
            length};
}
//=======================================================================================
subset_026::PositionReportPkg to_dto::parsePositionReportPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseBrusPkgHeader(data, bit_iterator).pkg_id;

    auto scale = get<uint8_t>(data, bit_iterator, VariableLength::Q_SCALE);
    auto last_balise_id = get<uint32_t>(data, bit_iterator, VariableLength::NID_LRBG);
    auto distance_passed = get<uint16_t>(data, bit_iterator, VariableLength::D_LRBG);
    auto balise_orientation = get<uint8_t>(data, bit_iterator, VariableLength::Q_DIRLRBG);
    auto balise_side = get<uint8_t>(data, bit_iterator, VariableLength::Q_DLRBG);
    auto upperConfidInt = get<uint16_t>(data, bit_iterator, VariableLength::L_DOUBTOVER);
    auto lowerConfidInt = get<uint16_t>(data, bit_iterator, VariableLength::L_DOUBTUNDER);
    auto train_integrity = get<bool>(data, bit_iterator, VariableLength::Q_LENGTH);
    auto length_in_cars = get<uint16_t>(data, bit_iterator, VariableLength::L_TRAININT);
    auto speed = get<uint8_t>(data, bit_iterator, VariableLength::V_TRAIN);
    auto is_nominal_direction = get<bool>(data, bit_iterator, VariableLength::Q_DIRTRAIN);
    auto control_mode = get<uint8_t>(data, bit_iterator, VariableLength::M_MODE);
    auto control_lvl = get<uint8_t>(data, bit_iterator, VariableLength::M_LEVEL);
    auto nid = get<uint8_t>(data, bit_iterator, VariableLength::NID_NTC);

    return {scale,
            last_balise_id,
            distance_passed,
            balise_orientation,
            balise_side,
            upperConfidInt,
            lowerConfidInt,
            train_integrity,
            length_in_cars,
            speed,
            is_nominal_direction,
            control_mode,
            control_lvl,
            nid};
}
//=======================================================================================
subset_026::TrainRunningNumberPkg to_dto::parseTrainRunningNumberPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseBrusPkgHeader(data, bit_iterator);

    auto number = get<uint32_t>(data, bit_iterator, VariableLength::NID_OPERATIONAL);

    return subset_026::TrainRunningNumberPkg{number};
}
//=======================================================================================
subset_026::ValidatedTrainDataPkg to_dto::parseValidatedTrainDataPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseBrusPkgHeader(data, bit_iterator);

    auto train_category = get<uint8_t>(data, bit_iterator, VariableLength::NC_CDTRAIN);
    auto train_type = get<uint16_t>(data, bit_iterator, VariableLength::NC_TRAIN);
    auto length_in_cars = get<uint16_t>(data, bit_iterator, VariableLength::L_TRAIN);
    auto max_speed = get<uint8_t>(data, bit_iterator, VariableLength::V_MAXTRAIN);
    auto loading_prof = get<uint8_t>(data, bit_iterator, VariableLength::M_LOADINGGAUGE);
    auto axis_load = get<uint8_t>(data, bit_iterator, VariableLength::M_AXLELOADCAT);
    auto is_airtight = get<bool>(data, bit_iterator, VariableLength::M_AIRTIGHT);
    auto axis_number = get<uint16_t>(data, bit_iterator, VariableLength::N_AXLE);

    auto iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);
    std::vector<subset_026::Axle> axis;

    for (int8_t i = 0; i < iter; ++i) {
        subset_026::Axle axle;

        axle.voltage = get<uint8_t>(data, bit_iterator, VariableLength::M_VOLTAGE);
        axle.nid = get<uint16_t>(data, bit_iterator, VariableLength::NID_CTRACTION);

        axis.emplace_back(std::move(axle));
    }

    auto nid_iter = get<uint8_t>(data, bit_iterator, VariableLength::N_ITER);
    std::vector<int8_t> nids;

    nids.reserve(nid_iter);

    for (int8_t j = 0; j < nid_iter; ++j)
        nids.emplace_back(get<uint8_t>(data, bit_iterator, VariableLength::NID_NTC));

    return {train_category,
            train_type,
            length_in_cars,
            max_speed,
            loading_prof,
            axis_load,
            is_airtight,
            axis_number,
            axis,
            nids};
}
//=======================================================================================
subset_026::AppDataFromBrusPkg to_dto::parseAppDataFromBrusPkg(const uint8_t *data, size_t &bit_iterator)
{
    parseBrusPkgHeader(data, bit_iterator);

    auto system_id = get<uint16_t>(data, bit_iterator, VariableLength::NID_XUSER);
    auto version = get<uint32_t>(data, bit_iterator, AppDataVarLength::MAP_VER);

    return {system_id, version};
}

}

}

}