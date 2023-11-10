#ifndef CRYPT_H_
#define CRYPT_H_

#include <stdlib.h>
#include <stdint.h>

#define OCTET 8

struct CRYPT
{
    static int cbcMac( const void       *data,
                        size_t          len,
                        const uint8_t   *key,
                        uint8_t         *mac    );

    static int calcMac( const void      *msg,
                        size_t          len,
                        const uint8_t   *ks,
                        const uint8_t   *da,
                        uint8_t         *mac    );

};

int sessionKey( const uint8_t    *ra,
                const uint8_t    *rb,
                const uint8_t    *kmac,
                uint8_t          *ksmac );

void random8( uint8_t *rnd );

uint16_t crcCcitt( const uint8_t *msg, size_t len );

uint_least32_t calcCrc32( const uint8_t *msg, size_t len );

#endif /* CRYPT_H_ */
