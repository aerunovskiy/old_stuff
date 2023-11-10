#ifndef RBC_GW_TODTO_H
#define RBC_GW_TODTO_H
//=======================================================================================
#include <bitset>
#include <binary_message.h>

#include "vlog.h"

#include "messages.h"
#include "constants.h"
namespace euroradio
{

namespace subset_026
{

namespace parsers
{

namespace to_dto
{
/**
 * Get bits from value without shift (i.e. taken bits would be first)
 * @tparam T type
 * @param value Source
 * @param start start bit
 * @param quantity number of bits
 * @return
 */
template<typename T>
inline T _getBits(T value, size_t start, size_t quantity)
noexcept
{

unsigned long long mask = (1u << quantity) - 1u;
//        if (quantity >= sizeof(size_t) )
//            return T(0);

//       unsigned long long mask = (static_cast<size_t>(1u) << quantity) - 1u;
return (

value &(mask

<< start)) >>

start;
}
//=======================================================================================
template<typename T>
T get(const uint8_t *data, size_t &start, size_t length)
{
    T number{};
    size_t byte = start / 8;

    uint8_t bitsLeftInByte = 8u - (start % 8u);

    start += length;

    while (length >= bitsLeftInByte) {
        T temp = _getBits(data[byte], 0, bitsLeftInByte);
        temp <<= length - bitsLeftInByte;
        number |= temp;

        length -= bitsLeftInByte;

        bitsLeftInByte = 8;
        ++byte;
    }

    size_t shift = bitsLeftInByte - length;
    number |= _getBits(data[byte], shift, length);

    return number;
}
//======================================== MESSAGES =====================================
//========================================= HEADERS =====================================
subset_026::Header parseHeader(const uint8_t *data, size_t &bit_iterator);
subset_026::BrusHeader parseBrusHeader(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::RbcHeader parseRbcHeader(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
//===================================== RBC -> BRUS =====================================
subset_026::SrAuthMsg parseSrAuthMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::MovementAuthorityMsg
parseMovementAuthorityMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::AckOfTrainDataMsg
parseAckOfTrainDataMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::UnconditionalESMsg
parseUnconditionalESMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::RevocationOfESMsg
parseRevocationOfESMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::GeneralMessage
parseGeneralMessage(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::SystemVersionMsg
parseSystemVersionMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::AckOfTerminationCSMsg
parseAckOfTerminationCSMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::TrainRejectedMsg
parseTrainRejectedMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::TrainAcceptedMsg
parseTrainAcceptedMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
//===================================== BRUS -> RBC =====================================
subset_026::ValidatedTrainDataMsg
parseValidatedTrainDataMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::MaRequestMsg parseMaRequestMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::TrainPositionReportMsg
parseTrainPositionReportMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::AckMsg parseAckMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::AckOfESMsg parseAckOfESMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::EndOfMissionMsg
parseEndOfMissionMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::NoCVSMsg parseNoCVSMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::InitiationCSMsg
parseInitiationCSMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::TerminationCSMsg
parseTerminationCSMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::StartOfMissionPrMsg
parseStartOfMissionPrMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::SessionEstablishedMsg
parseSessionEstablishedMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::ChannelStateMsg
parseChannelStateMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
subset_026::ActualDistanceMsg
parseActualDistanceMsg(const uint8_t *data, size_t &bit_iterator, const subset_026::Header &header);
//======================================== PACKAGES =====================================
struct BrusPkgHeader
{
    BrusPkgHeader(const uint8_t pkg_id,
                  const uint16_t pkg_length)
        : pkg_id(pkg_id),
          pkg_length(pkg_length)
    {}

    uint8_t pkg_id;
    uint16_t pkg_length;
};

struct RbcPkgHeader
{
    RbcPkgHeader(const uint8_t pkg_id,
                 const uint8_t data_direction,
                 const uint16_t pkg_length)
        : pkg_id(pkg_id),
          data_direction(data_direction),
          pkg_length(pkg_length)
    {}

    uint8_t pkg_id;
    uint8_t data_direction;
    uint16_t pkg_length;
};
//=======================================================================================
to_dto::BrusPkgHeader parseBrusPkgHeader(const uint8_t *data, size_t &bit_iterator);
to_dto::RbcPkgHeader parseRbcPkgHeader(const uint8_t *data, size_t &bit_iterator);
//===================================== BRUS -> RBC =====================================
subset_026::PositionReportPkg parsePositionReportPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::TrainRunningNumberPkg parseTrainRunningNumberPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::ValidatedTrainDataPkg parseValidatedTrainDataPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::AppDataFromBrusPkg parseAppDataFromBrusPkg(const uint8_t *data, size_t &bit_iterator);
//===================================== RBC -> BRUS =====================================
subset_026::LinkingPkg parseLinkingPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::MovementAuthPkg parseMovementAuthPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::LvlTransitionOrderPkg parseLvlTransitionOrderPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::SessionManagementPkg parseSessionManagementPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::AppDataFromRbcPkg parseAppDataFromRbcPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::ListOfBalisesInSrAuthPkg parseListOfBalisesInSrAuthPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::TemporarySpeedRestrPkg parseTemporarySpeedRestrPkg(const uint8_t *data, size_t &bit_iterator);
subset_026::TemporarySpeedRestrRevPkg parseTemporarySpeedRestrRevPkg(const uint8_t *data, size_t &bit_iterator);
//=======================================================================================
}

}

}

}
#endif