#ifndef SL_UTILS_H
#define SL_UTILS_H

#include "vbyte_buffer.h"

#include "vlog.h"

#include "stdint.h"

#include <string>
#include <sstream>
#include <cstring>
#include <des.h>

#include "ale.h"

// Класс SL_Utils представляет специфичные для SL вспомогательные методы
class SL_Utils
{

public:

    SL_Utils();

    static void genSessionKey(  uint64_t       ra,
                                uint64_t       rb,
                                const uint8_t  *kmac,
                                uint8_t        *ksmac );
};

#endif // SL_UTILS_H
