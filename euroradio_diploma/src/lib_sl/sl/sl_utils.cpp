#include "sl_utils.h"
#include "vbyte_buffer_view.h"

void SL_Utils::genSessionKey( uint64_t       ra,
                                uint64_t       rb,
                                const uint8_t  *kmac,
                                uint8_t        *ksmac )
{
    uint8_t ra_l[4], // Старшие 4 байта Random Number A
            ra_r[4], // Младшие 4 байта Random Number A

            rb_l[4], // Старшие 4 байта Random Number B
            rb_r[4]; // Младшие 4 байта Random Number B

    uint8_t secret_1[8],
            secret_2[8],
            secret_3[8];

    struct DES::des_ctx des_k1,
                        des_k2,
                        des_k3;

    uint8_t kmac1[8],
            kmac2[8],
            kmac3[8];

    uint8_t buf1[8],
            buf2[8];

    uint8_t ks1[8],
            ks2[8],
            ks3[8];

    uint8_t bytes_ra[sizeof (uint64_t)]; // Random Number A в виде массива байт
    uint8_t bytes_rb[sizeof (uint64_t)]; // Random Number B в виде массива байт

    for (size_t i = 0; i < sizeof(uint64_t); ++i)
    {
        bytes_ra[sizeof(uint64_t) - 1 - i] = static_cast<uint8_t>(ra >> 8*i);
        bytes_rb[sizeof(uint64_t) - 1 - i] = static_cast<uint8_t>(rb >> 8*i);
    }

    memcpy(ra_l, bytes_ra, 4);
    memcpy(ra_r, bytes_ra + 4, 4);

    memcpy(rb_l, bytes_rb, 4);
    memcpy(rb_r, bytes_rb + 4, 4);

    memcpy(secret_1, ra_l, 4);
    memcpy(secret_1 + 4, rb_l, 4);

    memcpy(secret_2, ra_r, 4);
    memcpy(secret_2 + 4, rb_r, 4);

    memcpy(secret_3, ra_l, 4);
    memcpy(secret_3 + 4, rb_l, 4);

    memcpy(kmac1, kmac, 8);
    memcpy(kmac2, kmac + 8, 8);
    memcpy(kmac3, kmac + 16, 8);

    DES::des_set_key(&des_k1, kmac1);
    DES::des_set_key(&des_k2, kmac2);
    DES::des_set_key(&des_k3, kmac3);

    if (des_k1.status != DES::DES_OK ||
        des_k2.status != DES::DES_OK ||
        des_k3.status != DES::DES_OK)
    {
        vfatal << "SL: Ошибка при генерации сессионных ключей.";
        return;
    }

    /*
     * The three 64-bit keys KS1, KS2 and KS3 are
     * calculated according the formulas:
     */
    DES::des_encrypt(&des_k1, 8, buf1, secret_1);
    DES::des_decrypt(&des_k2, 8, buf2, buf1);
    DES::des_encrypt(&des_k3, 8, ks1, buf2);

    DES::des_encrypt(&des_k1, 8, buf1, secret_2);
    DES::des_decrypt(&des_k2, 8, buf2, buf1);
    DES::des_encrypt(&des_k3, 8, ks2, buf2);

    DES::des_encrypt(&des_k3, 8, buf1, secret_3);
    DES::des_decrypt(&des_k2, 8, buf2, buf1);
    DES::des_encrypt(&des_k1, 8, ks3, buf2);

    memcpy(ksmac, ks1, 8);
    memcpy(ksmac + 8, ks2, 8);
    memcpy(ksmac + 16, ks3, 8);
}
