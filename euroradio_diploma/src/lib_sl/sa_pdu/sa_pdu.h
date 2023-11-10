#ifndef SA_PDU_H
#define SA_PDU_H

#include <stdint.h>
#include <string>

#include <endian.h>
#include <sys/param.h>

#include <arpa/inet.h>
#include "vbyte_buffer/vbyte_buffer.h"
#include <vlog.h>

#include <crypt.h>

#include "sl_utils.h"

// Message Type Identifier
enum MTI : uint8_t
{
    SA_AU1 = 0x01, // 0001
    SA_AU2 = 0x02, // 0010
    SA_AU3 = 0x03, // 0011
    SA_AR  = 0x09, // 1001
    SA_DT  = 0x05, // 0101
    SA_DI  = 0x08  // 1000
};

// ETCSID Type
enum ETY : uint8_t
{
    RADIO_INFILL_UNIT   = 0x00,
    RBC                 = 0x01,
    ENGINE              = 0x02,
    KEY_MNGMNT_ENT      = 0x05,
    INTERLOCKING        = 0x06
};

// Ключи для 3DES шифрования
const uint8_t k_ab[24] =
{
    0xA0, 0x92, 0x18, 0x7C, 0x36, 0x71, 0x28, 0x39,
    0x74, 0x63, 0x25, 0x16, 0xAC, 0x84, 0x59, 0x03,
    0x18, 0x26, 0x53, 0x47, 0x63, 0x8A, 0xCD, 0x20
};

// Direction Flag
enum DF : uint8_t
{
    TO_RESP,
    TO_INIT
};

// ETY + MTI + DF
union Header
{
    Header(uint8_t ety, uint8_t mti, uint8_t df)
    {
        field.ety  = ety;
        field.mti  = mti;
        field.df   = df;
    }

    Header(uint8_t u) : u8(u) { }

    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint8_t ety    : 3; // ETCS ID type of SA
        uint8_t mti    : 4;
        uint8_t df     : 1;
#else
        uint8_t df     : 1;
        uint8_t mti    : 4;
        uint8_t ety    : 3; // ETCS ID type of SA
#endif
    } field;

    uint8_t u8;
};

// SA + SaF
union SaSaF
{
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint32_t sa  : 24;
        uint8_t  saf : 8;
#else
        uint8_t  saf : 8;
        uint32_t sa  : 24;
#endif
    } field;

    uint32_t u32;

    SaSaF(uint32_t u) : u32(u) { }

    SaSaF(uint32_t sa, uint8_t saf)
    {
        field.sa = sa;
        field.saf = saf;
    }
};

struct Au1SaPdu
{
    // Конструктор для передачи
    Au1SaPdu(   uint8_t   ety,
                uint32_t  sa,
                uint64_t  rb    )
        : header(ety, MTI::SA_AU1, DF::TO_RESP),
          sasaf(sa, 1),
          rb(rb)
    { }

    // Конструктор для приёма
    Au1SaPdu(uint8_t    ety,
              uint8_t   mti,
              uint8_t   df,
              uint32_t  sa,
              uint8_t   saf,
              uint64_t  rb  )
        : header(ety, mti, df),
          sasaf(sa, saf),
          rb(rb)
    { }

    Header      header;
    SaSaF       sasaf;

    uint64_t    rb;        // Random number
};

struct Au2SaPdu
{
    // Конструктор для передачи
    Au2SaPdu( uint8_t   ety,
              uint32_t  sa,
              uint64_t  ra,
              uint64_t  rb,
              uint32_t  da  )
        : header(ety, MTI::SA_AU2, DF::TO_INIT),
          sasaf(sa, 1),
          ra(ra)
    {

        mac = calcMac(ra, rb, da);
    }

    // Конструктор для приёма
    Au2SaPdu(   uint8_t     ety,
                uint8_t     mti,
                uint8_t     df,
                uint32_t    sa,
                uint8_t     saf,
                uint64_t    ra,
                uint64_t    mac
                )
            : header(ety, mti, df),
              sasaf(sa, saf),
              ra(ra),
              mac(mac)
    {}

    uint64_t calcMac(uint64_t ra, uint64_t rb, uint64_t da)
    {
        uint32_t dar = static_cast<uint32_t>(da);
        uint8_t v[3];
        v[0] = dar & 0x0FF;
        dar >>= 8;
        v[1] = dar & 0x0FF;
        dar >>= 8;
        v[2] = dar & 0x0FF;
        dar >>= 8;

        // Формируем m
        vbyte_buffer m;
        m.append_BE(header.u8);
        m.append_BE(sasaf.u32);
        m.append_BE(ra);
        m.append_BE(rb);
        m.append_BE(v[2]);
        m.append_BE(v[1]);
        m.append_BE(v[0]);

        uint8_t k = v[2];
        v[2] = v[0];
        v[0] = k;

        uint8_t session_key[24];

        SL_Utils::genSessionKey(ra, rb, &k_ab[0], &session_key[0]);

        uint8_t mac[8];


        CRYPT::calcMac( &m.data()[0],
                        m.size(),
                        &session_key[0],
                        &v[0],
                        &mac[0]);

        uint64_t res = mac[7] | (static_cast<uint64_t>(mac[6]) << 8) |
                (static_cast<uint64_t>(mac[5])  << 16)|
                (static_cast<uint64_t>(mac[4])  << 24)|
                (static_cast<uint64_t>(mac[3])  << 32)|
                (static_cast<uint64_t>(mac[2])  << 40)|
                (static_cast<uint64_t>(mac[1])  << 48)|
                (static_cast<uint64_t>(mac[0])  << 56);

        return res;
    }

    Header      header;
    SaSaF       sasaf;

    uint64_t    ra;     // Random number
    uint64_t    mac;    // Message Authentication Code
};

struct Au3SaPdu
{
    // Конструктор для передачи
    Au3SaPdu(   uint64_t ra,
                uint64_t rb,
                uint64_t da     )
        : header(0, MTI::SA_AU3, DF::TO_RESP)
    {
        mac = calcMac(ra, rb, da);
    }

    // Конструктор для приёма
    Au3SaPdu(   uint8_t     ety,
                uint8_t     mti,
                uint8_t     df,
                uint64_t    mac)
        : header(ety, mti, df),
          mac(mac) {}

    uint64_t calcMac(uint64_t ra, uint64_t rb, uint64_t da)
    {
        // Получаем 3 байта ETCSID из DA
        uint32_t dar = static_cast<uint32_t>(da);
        uint8_t v[3];
        v[0] = dar & 0x0FF;
        dar >>= 8;
        v[1] = dar & 0x0FF;
        dar >>= 8;
        v[2] = dar & 0x0FF;
        dar >>= 8;

        // Формируем m
        vbyte_buffer m;

        // m = '000' | MTI | DF
        m.append_BE(header.u8);

        // m = '000' | MTI | DF | Rb | Ra
        m.append_BE(rb);
        m.append_BE(ra);

        // Изменяем порядок байт
        uint8_t k = v[2];
        v[2] = v[0];
        v[0] = k;

        // Генерируем сессионный ключ
        uint8_t session_key[24];

        SL_Utils::genSessionKey(ra, rb, &k_ab[0], &session_key[0]);

        uint8_t mac[8];

        // Вычисляем Message Authentication Code для данного ключа
        CRYPT::calcMac( m.data(),
                        m.size(),
                        &session_key[0],
                        &v[0],
                        &mac[0]);

        uint64_t res = mac[7] | (static_cast<uint64_t>(mac[6]) << 8) |
                (static_cast<uint64_t>(mac[5])  << 16)|
                (static_cast<uint64_t>(mac[4])  << 24)|
                (static_cast<uint64_t>(mac[3])  << 32)|
                (static_cast<uint64_t>(mac[2])  << 40)|
                (static_cast<uint64_t>(mac[1])  << 48)|
                (static_cast<uint64_t>(mac[0])  << 56);

        return res;
    }

    Header      header;
    uint64_t    mac;
};

struct ArSaPdu
{
    // Конструктор для приёма
    ArSaPdu( uint8_t   ety,
             uint8_t   mti,
             uint8_t   df,
             uint64_t  mac   )
        : header(ety, mti, df),
          mac  (mac)
    {}

    // Конструктор для передачи
    ArSaPdu( uint64_t  da,
             uint64_t  ra,
             uint64_t  rb   )
        : header(0, MTI::SA_AR, DF::TO_INIT)
    {
        mac =  calcMac(da, ra, rb);
    }

    uint64_t calcMac( uint64_t   da,
                      uint64_t   ra,
                      uint64_t   rb  )
    {
        uint32_t dar = static_cast<uint32_t>(da);
        uint8_t v[3];
        v[0] = dar & 0x0FF;
        dar >>= 8;
        v[1] = dar & 0x0FF;
        dar >>= 8;
        v[2] = dar & 0x0FF;
        dar >>= 8;

        // Формируем m
        vbyte_buffer m;

        m.append_BE(header.u8);

        uint8_t k = v[2];
        v[2] = v[0];
        v[0] = k;

        uint8_t session_key[24];

        SL_Utils::genSessionKey(ra, rb, &k_ab[0], &session_key[0]);

        uint8_t mac[8];

        CRYPT::calcMac( m.data(),
                        m.size(),
                        &session_key[0],
                        &v[0],
                        &mac[0]);

        uint64_t res = mac[7] | (static_cast<uint64_t>(mac[6]) << 8) |
                (static_cast<uint64_t>(mac[5])  << 16)|
                (static_cast<uint64_t>(mac[4])  << 24)|
                (static_cast<uint64_t>(mac[3])  << 32)|
                (static_cast<uint64_t>(mac[2])  << 40)|
                (static_cast<uint64_t>(mac[1])  << 48)|
                (static_cast<uint64_t>(mac[0])  << 56);

        return res;
    }

    Header      header;
    uint64_t    mac;
};

struct DtSaPdu
{
    // Конструктор для передачи
    DtSaPdu(    uint8_t         df,
                vbyte_buffer    user_data,
                uint64_t        da,
                uint64_t        ra,
                uint64_t        rb          )
        :   header(0, MTI::SA_DT, df),
            user_data(user_data)
    {
        mac = calcMac(da, ra, rb);
    }

    // Конструктор для приёма
    DtSaPdu(    uint8_t         ety,
                uint8_t         mti,
                uint8_t         df,
                vbyte_buffer    user_data,
                uint64_t        mac         )
        :   header(ety, mti, df),
            user_data(user_data),
            mac(mac) {}

    uint64_t calcMac(uint64_t   da,
                     uint64_t   ra,
                     uint64_t   rb)
    {

        uint32_t dar = static_cast<uint32_t>(da);
        uint8_t v[3];
        v[0] = dar & 0x0FF;
        dar >>= 8;
        v[1] = dar & 0x0FF;
        dar >>= 8;
        v[2] = dar & 0x0FF;
        dar >>= 8;

        // Формируем m
        vbyte_buffer m;

        m.append_BE( header.u8 );
        m.append( user_data );

        uint8_t k = v[2];
        v[2] = v[0];
        v[0] = k;

        uint8_t session_key[24];

        SL_Utils::genSessionKey(ra, rb, &k_ab[0], &session_key[0]);

        uint8_t mac[8];

        CRYPT::calcMac( m.data(),
                        m.size(),
                        &session_key[0],
                        &v[0],
                        &mac[0]);

        uint64_t res = mac[7] |
                (static_cast<uint64_t>(mac[6])  << 8 )|
                (static_cast<uint64_t>(mac[5])  << 16)|
                (static_cast<uint64_t>(mac[4])  << 24)|
                (static_cast<uint64_t>(mac[3])  << 32)|
                (static_cast<uint64_t>(mac[2])  << 40)|
                (static_cast<uint64_t>(mac[1])  << 48)|
                (static_cast<uint64_t>(mac[0])  << 56);

        return res;
    }


    Header          header;
    vbyte_buffer    user_data;
    uint64_t        mac;
};

struct DiSaPdu
{
    // Конструктор для передачи
    DiSaPdu(    uint8_t     df,
                uint8_t     reason,
                uint8_t     sub_reason)
        : header(0, MTI::SA_DI, df),
          reason(reason),
          sub_reason(sub_reason){}

    // Конструктор для приёма
    DiSaPdu(    uint8_t     ety,
                uint8_t     mti,
                uint8_t     df,
                uint8_t     reason,
                uint8_t     sub_reason)
        : header        (ety, mti, df),
          reason       (reason),
          sub_reason   (sub_reason){}

    Header      header;
    uint8_t     reason;
    uint8_t     sub_reason;
};

#endif // SA_PDU_H
