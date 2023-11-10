#ifndef SAPDU_PARSER_H
#define SAPDU_PARSER_H

#include "ale.h"
#include "sa_pdu.h"

// Класс предназначен для обработки сигналов, приходящих со слоя ALE, парсинга пакетов
// SaPDU, индикации типа приходящих пакетов
class SaPduParser
{

public:

    static const size_t     mac_size    = 8; // Размер поля MAC
    static const size_t     header_size = 1; // ETY + MTI + DF
    static const uint16_t   saf         = 1; // Safety Features

//================================= СИГНАЛЫ ==============================================
    VSignal<Au1SaPdu, uint16_t, uint32_t>    received_au1;
    VSignal<Au2SaPdu, uint16_t>              received_au2;
    VSignal<Au3SaPdu, uint16_t>              received_au3;
    VSignal<ArSaPdu , uint16_t>              received_ar;
    VSignal<DtSaPdu , uint16_t, uint8_t>     received_dt;

    VSignal<DiSaPdu , Ale::DisconnectIndicationData, uint16_t> received_di;
    VSignal<          Ale::DisconnectIndicationData, uint16_t> disconnect_indication;
//========================================================================================

    SaPduParser(std::shared_ptr<BasicAle> t);

private:

//======================= Действия по сигналам от ALE ====================================
    void onConnectIndication   (Ale::ConnectIndicationData    dt);
    void onConnectConfirmation (Ale::ConnectConfirmationData  dt);
    void onDataIndication      (Ale::DataIndicationData       dt);
    void onDisconnectIndication(Ale::DisconnectIndicationData dt);
//========================================================================================

    void parseAu1       ( vbyte_buffer user_data, uint16_t tcepid, uint32_t calling_etcsid );
    void parseAu2       ( vbyte_buffer user_data, uint16_t tcepid, uint32_t responding_etcsid );
    void parseAu3_Ar_Dt ( vbyte_buffer user_data, uint16_t tcepid, uint8_t channel );
    void parseDi        ( Ale::DisconnectIndicationData dt );

    std::shared_ptr<BasicAle> _t;
};

#endif // SAPDU_PARSER_H
