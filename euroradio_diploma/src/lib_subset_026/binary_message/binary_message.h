#ifndef RBC_GW_BINARYMESSAGE_H
#define RBC_GW_BINARYMESSAGE_H

#define VENDOR_MAYBE_UNUSED

#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>

namespace euroradio
{

namespace subset_26
{
    class BinaryMessage
    {
    public:
        explicit BinaryMessage(size_t length);

        explicit BinaryMessage(std::vector <uint8_t> data);

        BinaryMessage(uint8_t *data, size_t length);

        BinaryMessage(const BinaryMessage &msg);

        BinaryMessage(BinaryMessage &&msg)

        noexcept;

        ~BinaryMessage() = default;

        /**
         * @tparam T Value type
         * @param value Value to write
         * @param writeStart Where to start writing
         * @param length Length to write
         * @param bigEndian order
         * @return bits written
         */
        template<typename T>
        size_t Insert(T value, size_t writeStart, size_t length) {
            // If passed value requires more bits
            if (_leftmostSetBit(value) > length)
                return 0;

            size_t bitsLeftInByte = 8 - writeStart % 8; ///< bits left in current byte
            size_t bitsLeftToWrite = length; ///< remaining length to write

            while (bitsLeftToWrite > bitsLeftInByte) {
                bitsLeftToWrite -= bitsLeftInByte; // We count from right to left (cause bits)

                _data[writeStart / 8] |= _getBits(value, bitsLeftToWrite, bitsLeftInByte);

                writeStart += bitsLeftInByte;
                bitsLeftInByte = 8;
            }

            _data[writeStart / 8] |= _getBits(value, 0, bitsLeftToWrite) << (bitsLeftInByte - bitsLeftToWrite);

            return length;
        }

        /**
         * Writes value to the end of binary message
         * @param value Value to add
         * @param length In bits
         * @return number Added length
         */
        template<typename T>
        size_t Append(T value, size_t length) {
            size_t pos = Insert<T>(value, _bitCounter, length);
            _bitCounter += length;
            return pos;
        }

        /**
         * @return uint_8t data
         */
        VENDOR_MAYBE_UNUSED uint8_t *Data()

        noexcept;
        VENDOR_MAYBE_UNUSED char *DataInChar();

        std::string DataInString();

        /**
         * Get's value from array
         * @tparam T return type
         * @param start position of the first bit
         * @param length length of value
         * @return value
         */
        template<typename T>
        T Get(const size_t &start, size_t length) {
            size_t buff = start;
            return Get<T>(buff, length);
        }

        /**
         * Get's value from array
         * @tparam T return type
         * @param start position of the first bit
         * @param length length of value
         * @return value
         */
        template<typename T>
        T Get(size_t &start, size_t length) {
            T number{};
            size_t byte = start / 8;

            uint8_t bitsLeftInByte = 8u - (start % 8u);

            start += length;

            while (length >= bitsLeftInByte) {
                T temp = _getBits(_data[byte], 0, bitsLeftInByte);
                temp <<= length - bitsLeftInByte;
                number |= temp;

                length -= bitsLeftInByte;

                bitsLeftInByte = 8;
                ++byte;
            }

            size_t shift = bitsLeftInByte - length;
            number |= _getBits(_data[byte], shift, length);

            return number;
        }

        /**
         * @return size Current size in bits
         */
        VENDOR_MAYBE_UNUSED size_t Size() const

        noexcept;

        /**
         * Get bits from value without shift (i.e. taken bits would be first)
         * @tparam T type
         * @param value Source
         * @param start start bit
         * @param quantity number of bits
         * @return
         */
        template<typename T>
        inline static T _getBits(T value, size_t start, size_t quantity)

        noexcept
        {
            unsigned long long mask = (1u << quantity) - 1u;

            //TODO отладить
            //        if (quantity >= sizeof(size_t) )
            //            return T(0);

            // unsigned long long mask = (static_cast<size_t>(1u) << quantity) - 1u;
            return (value & (mask << start)) >> start;
        }

        /**
         * Finds index of leftmost set bit
         * @tparam T input type
         * @param value Where to search
         * @return index (from left (for example for 00(1)00101 it would be 5)
         */
        template<typename T>
        inline static size_t _leftmostSetBit(T value)

        noexcept
        {
            size_t msb = sizeof(T) * 8;

            for (; msb && !(value & (static_cast<size_t>(0x01) << (msb - 1))); --msb) {}

            return msb;
        }

        /**
         * Returns index of rightmost set bit
         * @tparam T input type
         * @param value input value
         * @return index of rightmost set bit
         */
        template<typename T>
        inline static size_t _rightmostSetBit(T value)

        noexcept
        {
            return log2(value & -value);
        }

        /**
         * @return true for little, false for big
         */
        static bool _littleEndian()

        noexcept
        {
            int num = 1;
            return *(char *) &num;
        }

        /**
         * Return n-th byte of value;
         * Endianess-dependent
         * @tparam T input type
         * @param value value to get byte from, value considered to be stored as big-endian
         * @param n number of byte to get
         * @return byte value
         */
        template<typename T>
        inline static uint8_t _getByte(T value, size_t n)

        noexcept
        {
            return ((uint8_t * )(&value))[n];
        }

        friend std::ostream &operator<<(std::ostream &os, const BinaryMessage &dt);

        std::vector <uint8_t> _data; ///< Actual data //TODO don't actually need vector, can be changed to uint8_t*
        size_t _bitCounter{}; ///< Last written bit
    };

    template<>
    inline size_t BinaryMessage::_leftmostSetBit<uint8_t>(uint8_t value) noexcept
    {
        size_t pos = 0;
        while (value >>= 1u)
        pos++;

        return
        pos;
    }

} //< subset_026

} //< euroradio

#endif //RBC_GW_BINARYMESSAGE_H