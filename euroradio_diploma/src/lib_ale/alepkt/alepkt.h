#ifndef ALEPKT_H
#define ALEPKT_H

#include <stdint.h>
#include <string>
#include <vbyte_buffer.h>

static const uint16_t   au1_au2_tsn     = 0;    // TSN always 0 for AU1,AU2
static const uint8_t    ale_version     = 0x01; // Версия ALE
static const uint8_t    q_of_service    = 0x03; // Always class D
static const uint8_t    app_type        = 0x1A; // Application Type

// Перечень возможных значений пакета ALEPKT
enum PACKET_TYPE: uint8_t
{
    AU1 = 1,
    AU2,
    DT,
    DI
};

// Состав заголовка пакета ALEPKT
// Пакеты типа DT и DI не имеют специфической (отличной от заголовка и UserData)
// части. Поэтому для них не были выделены отдельные классы.
class AlepktHeader
{
public:
    uint16_t     packet_length;                     // Длина пакета
    uint8_t      version            = ale_version;  // Версия библиотеки
    uint8_t      application_type   = app_type;     // Тип приложения
    uint16_t     tsn;                               // Номер пакета
    uint8_t      nr;                                // N/R флаг
    uint8_t      packet_type;                       // Типа пакета
    uint16_t     check_sum;                         // Контрольная сумма
    vbyte_buffer user_data;                         // Данные пользователя
};

struct AlepktAu1: public AlepktHeader
{
    AlepktAu1()
    {
        tsn         = au1_au2_tsn;
        packet_type = AU1;
    }

    AlepktAu1(AlepktHeader header)
        :AlepktHeader(header)
    {
        tsn         = au1_au2_tsn;
        packet_type = AU1;
    }

    uint32_t    calling_etcsid;     // Вызывающий ETCSID
    uint32_t    called_etcsid;      // Вызываемый ETCSID
    uint8_t     qos = q_of_service; // Тип соединения
};

struct AlepktAu2: public AlepktHeader
{
    AlepktAu2()
    {
        tsn         = au1_au2_tsn;
        packet_type = AU2;
    }

    AlepktAu2(AlepktHeader header)
        :AlepktHeader(header)
    {
        tsn         = au1_au2_tsn;
        packet_type = AU2;
    }

    uint32_t    responding_etcsid;  // Отвечающий на запрос соединения ETCSID
};

#endif // ALEPKT_H
