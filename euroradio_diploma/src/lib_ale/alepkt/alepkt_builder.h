#ifndef ALEPKT_BUILDER_H
#define ALEPKT_BUILDER_H

#include "alepkt.h"
#include "alepkt_parser.h"
#include "vbyte_buffer.h"
#include "vlog.h"

#include "vbyte_buffer_view.h"

// Класс ALEPKT_Builder предназначен для формирования последовательности байт в виде
// строки vbyte_buffer из получаемых пакетов ALEPKT.
class AlepktBuilder
{
public:
    // Размер данных, по которым вычисляется контрольная сумма
    static const int alepkt_crc_size        =  8;

    // Размер поля контрольной суммы в ALEPKT
    static const int alepkt_crc_field_size      =  2;
    static const int alepkt_header_size         = 10;
    static const int alepkt_packet_length_size  =  2;
    static const int alepkt_au1_size            =  9;
    static const int alepkt_au2_size            =  4;

    static vbyte_buffer formAlepktAu1(AlepktAu1 pkt)
    {

//      Вставляем в ALEPKT AU1 заголовок
//========================================================================================
        vbyte_buffer pkg;

        // Вставляем в начало PacketLength
        pkg.append_BE( static_cast<uint16_t>(   pkt.user_data.size() +
                                                alepkt_au1_size     +
                                                alepkt_header_size  -
                                                alepkt_packet_length_size ) );
        pkg.append_BE(pkt.version);
        pkg.append_BE(pkt.application_type);
        pkg.append_BE(pkt.tsn);
        pkg.append_BE(pkt.nr);
        pkg.append_BE(pkt.packet_type);

        // Вставляем в конец заголовка CheckSum
        pkg.append_BE(AlepktParser::crc16(pkg));
//========================================================================================

//                  Вставляем характерные для AU1 разделы
//========================================================================================
        pkg.append_BE(pkt.calling_etcsid);
        pkg.append_BE(pkt.called_etcsid);
        pkg.append_BE(pkt.qos);
//========================================================================================

        // Вставляем User Data
        pkg.append(pkt.user_data);

        vtrace << "ALE: Сформирован пакет ALEPKT AU1:"; // << pkg.toHex();

        return pkg;
    }

    static vbyte_buffer formAlepktAu2(AlepktAu2 pkt)
    {

//      Вставляем в ALEPKT AU1 заголовок
//========================================================================================
        vbyte_buffer pkg;

        // Вставляем в начало PacketLength
        pkg.append_BE( static_cast<uint16_t>(   pkt.user_data.size() +
                                                alepkt_au2_size     +
                                                alepkt_header_size  -
                                                alepkt_packet_length_size ) );
        pkg.append_BE(pkt.version);
        pkg.append_BE(pkt.application_type);
        pkg.append_BE(pkt.tsn);
        pkg.append_BE(pkt.nr);
        pkg.append_BE(pkt.packet_type);

        // Вставляем в конец заголовка CheckSum
        pkg.append_BE(AlepktParser::crc16(pkg));
//========================================================================================

        // Вставляем характерные для AU2 разделы
        pkg.append_BE(pkt.responding_etcsid);

        // Вставляем User Data
        pkg.append(pkt.user_data);

        vtrace << "ALE: Сформирован пакет ALEPKT AU2:";// << pkg.toHex();

        return pkg;
    }

    static vbyte_buffer formAlepktDtDi(AlepktHeader pkt)
    {

//      Вставляем в ALEPKT DT/DI заголовок
//========================================================================================
        vbyte_buffer pkg;

        // Вставляем в начало PacketLength
        pkg.append_BE( static_cast<uint16_t>(   pkt.user_data.size() +
                                                alepkt_header_size  -
                                                alepkt_packet_length_size ) );
        pkg.append_BE(pkt.version);
        pkg.append_BE(pkt.application_type);
        pkg.append_BE(pkt.tsn);
        pkg.append_BE(pkt.nr);
        pkg.append_BE(pkt.packet_type);
//========================================================================================

        // Вставляем в конец заголовка CheckSum
        pkg.append_BE(AlepktParser::crc16(pkg));

        // Вставляем User Data
        pkg.append(pkt.user_data);

        // В зависимости от типа пакета логируем формирование пакета
        if (pkt.packet_type == PACKET_TYPE::DT)
            vtrace << "ALE: Сформирован пакет ALEPKT DT:";// << pkg.toHex();
        else
            vtrace << "ALE: Сформирован пакет ALEPKT DI:";// << pkg.toHex();

        return pkg;

    }
};
#endif // ALEPKT_BUILDER_H
