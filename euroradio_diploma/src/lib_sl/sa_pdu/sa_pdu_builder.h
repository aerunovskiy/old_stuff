#ifndef SAPDU_BUILDER_H
#define SAPDU_BUILDER_H

#include "sa_pdu.h"
#include <string>
#include <vbyte_buffer.h>

// Класс SaPDU_builder предназначен для формирования строки vbyte_buffer из пакетов SaPDU
// для последующей их передачи. Форматы пакетов представлены в SS-037 7.2.5
class SaPduBuilder
{

public:

    static vbyte_buffer formAu1(Au1SaPdu au1)
    {
        vbyte_buffer data;        

        data.append_BE(au1.header.u8);
        data.append_BE(au1.sasaf.u32);
        data.append_BE(au1.rb);

        vtrace << "SL: Сформирован пакет SaPDU AU1:";// << data.toHex();

        return data;
    }

    static vbyte_buffer formAu2(Au2SaPdu au2)
    {
        vbyte_buffer data;

        data.append_BE(au2.header.u8);
        data.append_BE(au2.sasaf.u32);
        data.append_BE(au2.ra);
        data.append_BE(au2.mac);

        vtrace << "SL: Сформирован пакет SaPDU AU2:";// << data.toHex();

        return data;
    }

    static vbyte_buffer formAu3(Au3SaPdu au3)
    {
        vbyte_buffer data;

        data.append_BE(au3.header.u8);
        data.append_BE(au3.mac);

        vtrace << "SL: Сформирован пакет SaPDU AU3:";// << data.toHex();

        return data;
    }

    static vbyte_buffer formAr(ArSaPdu ar)
    {
        vbyte_buffer data;

        data.append_BE(ar.header.u8);
        data.append_BE(ar.mac);

        vtrace << "SL: Сформирован пакет SaPDU AR:";// << data.toHex();

        return data;
    }

    static vbyte_buffer formDt(DtSaPdu dt)
    {
        vbyte_buffer data;

        data.append_BE(dt.header.u8);
        data.append(dt.user_data);
        data.append_BE(dt.mac);

        vtrace << "SL: Сформирован пакет SaPDU DT:";// << data.toHex();

        return data;
    }

    static vbyte_buffer formDi(DiSaPdu di)
    {
        vbyte_buffer data;

        data.append_BE(di.header.u8);
        data.append_BE(di.reason);
        data.append_BE(di.sub_reason);

        vtrace << "SL: Сформирован пакет SaPDU DI:";// << data.toHex();

        return data;
    }

};

#endif // SAPDU_BUILDER_H
