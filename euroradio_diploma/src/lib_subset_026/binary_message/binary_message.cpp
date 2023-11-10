#include "binary_message.h"
//=======================================================================================
#include <utility>
//=======================================================================================
namespace euroradio
{

namespace subset_26
{
        BinaryMessage::BinaryMessage(size_t length) : _bitCounter(0), _data(length, 0) {}
        BinaryMessage::BinaryMessage(uint8_t *data, size_t length) : _data(data, data + length), _bitCounter(0) {}
        BinaryMessage::BinaryMessage(std::vector <uint8_t> data) : _data(std::move(data)), _bitCounter(0) {}
        BinaryMessage::BinaryMessage(const BinaryMessage &msg) = default;
        BinaryMessage::BinaryMessage(BinaryMessage &&msg) noexcept :
        _data(std::move(msg._data)),
        _bitCounter(msg._bitCounter)
        {}

        VENDOR_MAYBE_UNUSED
        uint8_t *BinaryMessage::Data() noexcept
        {
            return _data.data();
        }

        VENDOR_MAYBE_UNUSED
        size_t BinaryMessage::Size() const noexcept
        {
            return _bitCounter;
        }

        VENDOR_MAYBE_UNUSED
        char *BinaryMessage::DataInChar()
        {
            return reinterpret_cast<char *>( _data.data());
        }

        std::string BinaryMessage::DataInString()
        {
            return std::string(_data.begin(), _data.end());
        }

        std::ostream &operator<<(std::ostream &os, const BinaryMessage &msg)
        {
            for (const auto &b : msg._data)
                os << std::bitset<8>(b) << '\'';
            return os;
        }

}

}