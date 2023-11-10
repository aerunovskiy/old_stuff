#ifndef SL_H
#define SL_H

#include <vsignal.h>
#include <vtimer.h>
#include <vsettings.h>

#include "sa_pdu.h"
#include "sa_pdu_parser.h"

#include "ale_controller.h"

// Класс SL реализует логику работы безопасного слоя Safety Layer (SS-037 Глава 7)
class Sl
{
    uint8_t  app_type;  // Значение поля Application Type
    uint32_t t_estab;    // Время на установление соединения на уровне SL (мс)

    // Таблица состояний слоя SL
    enum STATES
    {
        WFTC,       // Wait for transport connection
        WFAR,       // Wait for the authentication response SaPDU
        DATA,       // Safety connection is opened and ready for data transfer
        WFAU3,      // Wait for the third authentication message
        WFRESP,     // Wait for Sa-CONNECT.response
        IDLE        // Safety connection is closed or does not exist
    };


public:

    const uint16_t CLIENT_TCEPID = 100;

    std::string convertStateToString( STATES state )
    {
        switch (state)
        {
            case WFTC   :
            {
                return "WFTC";
            }
            case WFAR   :
            {
                return "WFAR";
            }
            case DATA   :
            {
                return "DATA";
            }
            case WFAU3  :
            {
                return "WFAU3";
            }
            case WFRESP :
            {
                return "WFRESP";
            }
            case IDLE   :
            {
                return "IDLE";
            }
            default:
            {
                return "UNKNOWN_STATE";
            }
        }
    }

    // Параметры соединения для каждого TCEPID на уровне SL
    struct SlConnectParams
    {
        SlConnectParams()
            : timer_estab(std::make_shared<VTimer>())
        { }

        uint8_t     df          = 0;
        uint32_t    peer_etcsid = 0;
        uint64_t    ra          = 0; // Random Number A
        uint64_t    rb          = 0; // Random Number B

        std::shared_ptr<VTimer> timer_estab = nullptr;
    };

    // Структура данных, передаваемых в сигнале DataIndication уровня SL
    struct DataIndicationData
    {
        DataIndicationData(uint16_t tcepid, vbyte_buffer userdata, uint8_t channel)
            : tcepid(tcepid),
              user_data(userdata),
              channel(channel)
              {}

        uint16_t     tcepid;
        vbyte_buffer user_data;
        bool         channel;
    };

    // Структура данных, передаваемых в сигналах DisconnectIndication,
    // либо ReportIndication слоя SL
    struct DisRepIndicationData
    {
        DisRepIndicationData(uint16_t tcepid, uint8_t r, uint8_t sr)
            :   tcepid(tcepid),
                reason(r),
                sub_reason(sr) {}

        uint16_t    tcepid;
        uint8_t     reason;
        uint8_t     sub_reason;
    };

    Sl( const vsettings &settings );

//============================= СИГНАЛЫ ==================================================
    VSignal<uint16_t>                           connect_indication;
    VSignal<uint16_t>                           connect_confirmation;
    VSignal<DataIndicationData>                 data_indication;
    VSignal<uint16_t, uint32_t>                 ale_connect_indication;
    VSignal<uint16_t, uint32_t>                 ale_connect_confirmation;

    VSignal< DisRepIndicationData,
             Ale::DisconnectIndicationData >    disconnect_indication;

    VSignal<DisRepIndicationData>               report_indication;
    VSignal<bool>                               active_channel;
//========================================================================================

//============================ ДЕЙСТВИЯ ==================================================
    void connectRequest(uint32_t responding_etcsid);

    void connectResponse(uint16_t tcepidxmt);

    void dataRequest(uint16_t       tcepidxmt,
                     std::string    user_data);

    void disconnectRequest(const uint16_t tcepidxmt,
                           uint8_t  reason,
                           uint8_t  sub_reason);

    void disconnectByPeer( uint32_t peer_etcsid,
                           uint8_t  reason,
                           uint8_t  sub_reason  );

//========================================================================================

private:

//======================= ДЕЙСТВИЯ ПО СИГНАЛАМ ===========================================
    void onAu1Received  ( Au1SaPdu,  uint16_t tcepid, uint32_t calling_etcsid);
    void onAu2Received  ( Au2SaPdu,  uint16_t tcepid );
    void onAu3Received  ( Au3SaPdu,  uint16_t tcepid );
    void onArReceived   ( ArSaPdu,   uint16_t tcepid );
    void onDtReceived   ( DtSaPdu,   uint16_t tcepid, uint8_t channel );

    void onDiReceived          ( DiSaPdu, Ale::DisconnectIndicationData, uint16_t tcepid );
    void onDisconnectIndication( Ale::DisconnectIndicationData, uint16_t tcepid );
    void onActiveChannel       ( bool channel );
//========================================================================================

    inline uint32_t getEtcsidByTcepid( const uint16_t& tcepid ) const
    {
        for ( const auto& element : _con_params )
        {
            if (element.first == tcepid)
            {
                return element.second.peer_etcsid;
            }
        }

        return 0;
    }

    inline const uint16_t getTcepidByEctsid( const uint32_t& etcsid ) const
    {
        for ( auto& element : _con_params )
        {
            if (element.second.peer_etcsid == etcsid)
                return element.first;
        }

        return 101;
    }

    std::unique_ptr<VTimer> _timer_estab;    // Duration of a connect establishment

    // Параметры данной сущности
    uint8_t     _ety;                   // ETCS ID Type
    uint32_t    _etcsid;                // ETCS ID

    int     _connection_type;       // Связь с МПЦ или БРУС

    std::shared_ptr<BasicAle> _ale;     // Объект низлежащего слоя ALE    

    std::shared_ptr<SaPduParser> _parser;

    // Таблица параметров соединений
    std::map<uint16_t, SlConnectParams>  _con_params;

    // Таблица полученных Random Number B, ожидающих установления соответствующего
    // соединения
    std::map<uint32_t, uint64_t>        _wait_rb;

    // Статусы для каждого ETCSID
    std::map<uint32_t, STATES>  _states;

    // Таблица переменных времени установления соединения, ожидающих установления
    // соответствующего соединения
    std::map<uint32_t, uint32_t>        _wait_t_estab;
    std::map<uint32_t, std::shared_ptr<VTimer>> _wait_timers;
};

#endif // SL_H
