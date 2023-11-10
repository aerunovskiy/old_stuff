#ifndef DES_H_
#define DES_H_

#define DES_KEY_SIZE 8
#define DES_BLOCK_SIZE 8
#define _DES_KEY_LENGTH 32
#define DES3_KEY_SIZE 24
#define DES3_BLOCK_SIZE DES_BLOCK_SIZE

#include <stdint.h>

class DES
{
    public:

        enum des_error { DES_OK, DES_BAD_PARITY, DES_WEAK_KEY };

        struct des_ctx
        {
            uint32_t        key[_DES_KEY_LENGTH];
            enum des_error  status;
        };

        /* On success, returns 1 and sets ctx->status to DES_OK (zero). On
         * error, returns 0 and sets ctx->status accordingly. */
        static int des_set_key( struct des_ctx   *ctx,
                                const uint8_t    *key );

        static void des_encrypt( const struct des_ctx   *ctx,
                                 unsigned               length,
                                 uint8_t                *dst,
                                 const uint8_t          *src );

        static void des_decrypt( const struct des_ctx   *ctx,
                                 unsigned               length,
                                 uint8_t                *dst,
                                 const uint8_t          *src );

        static void des_fix_parity( unsigned        length,
                                    uint8_t         *dst,
                                    const uint8_t   *src );

        struct des3_ctx
        {
            struct des_ctx des[3];
            enum des_error status;
        };

        /* On success, returns 1 and sets ctx->status to DES_OK (zero). On
         * error, returns 0 and sets ctx->status accordingly. */

        static int des3_set_key( struct des3_ctx    *ctx,
                                 const uint8_t      *key );

        static void des3_encrypt( const struct des3_ctx *ctx,
                                  unsigned              length,
                                  uint8_t               *dst,
                                  const uint8_t         *src );

        static void des3_decrypt( const struct des3_ctx *ctx,
                                  unsigned              length,
                                  uint8_t               *dst,
                                  const uint8_t         *src );
};

#endif /* DES_H_ */
