#ifndef ALEPKT_PARSER_H
#define ALEPKT_PARSER_H

#include "vbyte_buffer.h"
#include "alepkt.h"
#include "vsignal.h"

// Класс ALEPKT_Parser предназначен для парсинга принимаемых ALE пакетов, определения их
// типа, логирования ошибок в формате или составе пакета, вычисления контрольной суммы
// пакета, сшивания единого пакета из последовательных TCP посылок.
class AlepktParser
{

public:

//===================== СИГНАЛЫ ==========================================================
    VSignal<AlepktAu1>       received_au1; // Получен пакет ALEPKT AU1
    VSignal<AlepktAu2>       received_au2; // Получен пакет ALEPKT AU2
    VSignal<AlepktHeader>    received_dt;  // Получен пакет ALEPKT DT
    VSignal<AlepktHeader>    received_di;  // Получен пакет ALEPKT DI
//========================================================================================

    // Добавление полученных по TCP пакетов байт в буфер
    void appendData(vbyte_buffer data);

    inline void clearBuf() { _buf.clear(); }

    // Вычисление контрольной суммы CRC16-CCITT
    static uint16_t crc16(vbyte_buffer s);

private:

    // Метод для парсинга пакета ALE, сформированного из полученных по TCP пакетов байт
    void parseAlepkt(vbyte_buffer& alepkt);

    // Буфер для принимаемых по TCP пакетов байт
    vbyte_buffer  _buf;

    static const size_t _mtu = 65000;    // Maximum Transmission Unit (SS-098 6.8.2.2.3)

    static const int _crc_size = 8;      // Размер строки (байт), для которой вычисляется
                                        // контрольная сумма (SS-098 6.4.5.1.5)

    static const int _alepkt_header_size = 10;  // Размер заголовка пакета ALEPKT
    static const int _alepkt_au1_size    = 9;   // Размер специфичной для ALEPKT AU1 части
    static const int _alepkt_au2_size    = 4;   // Размер специфичной для ALEPKT AU2 части

//== Специфичные для типа пакета методы парсинга, вызываемые из метода Parse_ALEPKT ======
    AlepktAu1      parseAu1     (AlepktHeader header, vbyte_buffer pkt);
    AlepktAu2      parseAu2     (AlepktHeader header, vbyte_buffer pkt);
    AlepktHeader   parseDtDi    (AlepktHeader header, vbyte_buffer pkt);
//========================================================================================

};

#endif // ALEPKT_PARSER_H
