#include "alepkt_parser.h"
#include "vlog.h"
#include "vbyte_buffer_view.h"

void AlepktParser::appendData(vbyte_buffer data)
{
    // Добавляем данные
    _buf.append(data);

    // Если есть PacketLength
    while (_buf.size() > 1 &&_buf.size() >= _buf.view().u16_BE() + 2)
    {
        // Достаем ALEPKT из буфера и парсим
        vbyte_buffer alepkt(_buf.left(_buf.view().u16_BE() + 2));

        _buf.chop_front(_buf.view().u16_BE() + 2);

        vtrace << "ALE: Получен пакет ALEPKT:";// << alepkt.toHex();

        parseAlepkt(alepkt);

    }
}

void AlepktParser::parseAlepkt(vbyte_buffer& alepkt)
{
    //vtrace << "Заголовок ALEPKT:";

    // Получаем данные заголовка
    AlepktHeader header;

    auto view = alepkt.view();

    header.packet_length    =   view.u16_BE();
    header.version          =   view.u8();
    header.application_type =   view.u8();
    header.tsn              =   view.u16_BE();
    header.nr               =   view.u8();
    header.packet_type      =   view.u8();
    header.check_sum        =   view.u16_BE();

//    vtrace << "Packet Length ="
//           << std::hex << header.packet_length                       << "\n"
//           << "\t\t\t\tVersion ="
//           << std::hex << static_cast<int>(header.version)          << "\n"
//           << "\t\t\t\tApplicationType ="
//           << std::hex << static_cast<int>(header.application_type)  << "\n"
//           << "\t\t\t\tTSN ="
//           << std::hex << header.tsn                                << "\n"
//           << "\t\t\t\tNR ="
//           << std::hex << static_cast<int>(header.nr)               << "\n"
//           << "\t\t\t\tPacketType = "
//           << std::hex << static_cast<int>(header.packet_type)       << "\n"
//           << "\t\t\t\tCheckSum ="
//           << std::hex << header.check_sum                           << "\n";

    // Проверяем контрольную сумму
    if (header.check_sum != crc16(alepkt.left(_crc_size)))
    {
        vwarning << "ALE: Не пройдена проверка контрольной суммы. Сообщение отклоняется";
        return;
    }

    // Отрезаем у пакета заголовок (больше не нужен)
    alepkt.chop_front(_alepkt_header_size);

    // В завимисости от типа пакета вызываем соответствующие сигналы
    switch (header.packet_type)
    {
        case PACKET_TYPE::AU1:
        {
            if (header.tsn != 0)
            {
                vfatal << "ALE: TSN у ALEPKT AU1 должен быть равен 0."
                       << "Полученный TSN =" << header.tsn;
                break;
            }

            if (alepkt.size() < _alepkt_au1_size)
            {
                vfatal << "ALE: Неверный размер пакета AU1";
                break;
            }

            vtrace << "ALE: Получен пакет ALEPKT AU1";
            received_au1(parseAu1(header, alepkt));
            break;
        }
        case PACKET_TYPE::AU2:
        {
            if (header.tsn != 0)
                vfatal << "ALE: TSN у ALEPKT AU2 должен быть равен 0."
                       << "Полученный TSN =" << header.tsn;

            vtrace << "ALE: Получен пакет ALEPKT AU2";
            received_au2(parseAu2(header, alepkt));
            break;
        }
        case PACKET_TYPE::DT:
        {
            vtrace << "ALE: Получен пакет ALEPKT DT";
            received_dt(parseDtDi(header, alepkt));
            break;
        }
        case PACKET_TYPE::DI:
        {
            vtrace << "ALE: Получен пакет ALEPKT DI";
            received_di(parseDtDi(header, alepkt));
            break;
        }

        default:
        {
            vwarning << "ALE: Получен неверный тип ALEPKT";
        }
    }
}

uint16_t AlepktParser::crc16(vbyte_buffer s)
{
    uint16_t crc = 0xFFFF;

    for (char ch : s.str())
    {
        crc ^= ch << 8;

        for (int i = 0; i < 8; i++)
            crc = crc & 0x8000 ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                               : static_cast<uint16_t> (crc << 1);
    }

    return crc;
}

AlepktAu1 AlepktParser::parseAu1 (AlepktHeader header, vbyte_buffer pkt)
{
    //vtrace << "ALE: Специальная часть пакета ALEPKT AU1:";
    AlepktAu1 au1(header);

    auto view = pkt.view();

    au1.calling_etcsid = view.u32_BE() & 0x00FFFFFF;
    au1.called_etcsid  = view.u32_BE() & 0x00FFFFFF;
    au1.qos            = view.u8();

    if (au1.qos != 0x03)
    {
        vwarning << "ALE: Неверный QoS";
    }

    pkt.chop_front(_alepkt_au1_size);

    if (pkt.size() > _mtu)
    {
        vwarning << "ALE: Размер пакета ALEPKT AU1 больше максимального";
    }

    au1.user_data = pkt;

//    vtrace << "Calling ETCSID =" << std::hex << au1.calling_etcsid;
//    vtrace << "Called ETCSID ="  << std::hex << au1.called_etcsid;
//    vtrace << "QoS ="            << std::hex << static_cast<int>(au1.qos);

    return au1;

}

AlepktAu2 AlepktParser::parseAu2  (AlepktHeader header, vbyte_buffer pkt)
{
    //vtrace << "ALE: Специальная часть пакета ALEPKT AU2:";

    AlepktAu2 au2(header);

    auto view = pkt.view();

    au2.responding_etcsid = view.u32_BE() & 0x00FFFFFF;

    pkt.chop_front(_alepkt_au2_size);

    //vtrace << "Responding ETCSID =" << std::hex << au2.responding_etcsid;

    if (pkt.size() > _mtu)
    {
        vwarning << "ALE: Размер пакета ALEPKT AU2 больше максимального";
    }

    au2.user_data = pkt;

    return au2;

}

AlepktHeader AlepktParser::parseDtDi (AlepktHeader header, vbyte_buffer pkt)
{
    if (pkt.size() > _mtu)
    {
        if (header.packet_type == PACKET_TYPE::DT)
            vwarning << "ALE: Размер пакета ALEPKT DT больше максимального";
        else if (header.packet_type == PACKET_TYPE::DI)
            vwarning << "ALE: Размер пакета ALEPKT DI больше максимального";
    }

    header.user_data = pkt;
    return header;
}
