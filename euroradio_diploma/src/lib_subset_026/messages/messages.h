#ifndef RBC_GW_MESSAGES_H
#define RBC_GW_MESSAGES_H

#include <memory>

#include "packages.h"

namespace euroradio
{

namespace subset_026
{
//======================================== HEADERS ======================================
    /**
     * Общее поле всех сообщений
     */
    struct Header
    {
        Header(const uint8_t  id,
               const uint16_t length,
               const uint32_t time)
        : id    (id),
          length(length),
          time  (time)
        {};

        uint8_t  id;
        uint16_t length;
        uint32_t time;   ///< Время отправки сообщения (T_TRAIN)
    };
//=======================================================================================
    /**
     * Заголовок сообщений БРУС -> РБЦ
     */
    struct BrusHeader: public Header
    {
        BrusHeader(const Header   &header,
                   const uint32_t brus_id)
        : Header (header),
          brus_id(brus_id)
        {}

        uint32_t brus_id; ///< ID БРУС (NID_ENGINE)
    };
//=======================================================================================
    /**
     * Заголовок сообщений РБЦ -> БРУС
     */
    struct RbcHeader : public Header
    {
        RbcHeader(const Header   &header,
                  const bool     ack,
                  const uint32_t last_balise_id)
        : Header        (header),
          ack           (ack),
          last_balise_id(last_balise_id)
        {}

        bool     ack;            ///< Флаг запроса подтверждения (M_ACK)
        uint32_t last_balise_id; ///< Идентификатор последней пройденной бализы (NID_LRBG)
    };
//================================== BRUS->RBC MESSAGES =================================
    /**
     * Сообщение 129 Validated Train ValidatedTrainDataPkg
     */
    struct ValidatedTrainDataMsg
    {
        ValidatedTrainDataMsg(const BrusHeader            &header,
                              const PositionReportPkg     &position,
                              const TrainRunningNumberPkg &number,
                              const ValidatedTrainDataPkg &data)
        : header  (header),
          position(position),
          number  (number),
          data    (data)
        {}

        BrusHeader            header;
        PositionReportPkg     position;
        TrainRunningNumberPkg number;
        ValidatedTrainDataPkg data;
    };
//=======================================================================================
    /**
     * Сообщение 132 MA Request
     */
    struct MaRequestMsg
    {
        MaRequestMsg(const BrusHeader        &header,
                     const uint8_t            reason,
                     const PositionReportPkg &position)
        : header  (header),
          reason  (reason),
          position(position)
        {}

        BrusHeader        header;
        uint8_t            reason;   ///< Причина запроса Q_MARQSTREASON
        PositionReportPkg position;
    };
//=======================================================================================
    /**
     * Сообщение 136 Train PositionReportPkg Report
     */
    struct TrainPositionReportMsg
    {
        TrainPositionReportMsg(const BrusHeader            &header,
                               const PositionReportPkg     &position,
                               const TrainRunningNumberPkg &number)
        : header  (header),
          position(position),
          number  (number)
        {}

        BrusHeader            header;
        PositionReportPkg     position;
        TrainRunningNumberPkg number;
    };
//=======================================================================================
    /**
     * Сообщение 146 AckMsg
     */
    struct AckMsg
    {
        AckMsg(const BrusHeader &header,
               const uint32_t   confirmation_time)
        : header           (header),
          confirmation_time(confirmation_time)
        {}

        BrusHeader header;
        uint32_t   confirmation_time; ///< Время отправки подтверждаемого сообщения (T_TRAIN)
    };
//=======================================================================================
    /**
     * Сообщение 147 AckMsg of Emergency Stop
     */
    struct AckOfESMsg
    {
        AckOfESMsg(const BrusHeader        &header,
                   const uint8_t           es_id,
                   const bool              is_stop,
                   const PositionReportPkg &position)
        : header  (header),
          es_id   (es_id),
          is_stop (is_stop),
          position(position)
        {}

        BrusHeader        header;
        uint8_t            es_id;    ///< Идентификатор сообщения аварийной остановки (NID_EM)
        bool              is_stop;  ///< Результат подтверждения аварийной остановки (Q_EMERGENCYSTOP)
        PositionReportPkg position;
    };
//=======================================================================================
    /**
     * Сообщение 150 End of Mission
     */
    struct EndOfMissionMsg
    {
        EndOfMissionMsg(const BrusHeader        &header,
                        const PositionReportPkg &position)
        : header  (header),
          position(position)
        {}

        BrusHeader        header;
        PositionReportPkg position;
    };
//=======================================================================================
    /**
     * Сообщение 154 No compatible version supported
     */
    struct NoCVSMsg
    {
        NoCVSMsg(const BrusHeader &header)
        : header(header)
        {}

        BrusHeader header;
    };
//=======================================================================================
    /**
    * Сообщение 155 Initiation of a communication session
    */
    struct InitiationCSMsg
    {
        // Need for pending in SS-026
        InitiationCSMsg()
            :   header(Header(0,0,0),0),
                number(0)
        {}

        InitiationCSMsg(const BrusHeader &header,
                        const uint32_t    number)
        : header(header),
          number(number)
        {}

        BrusHeader            header;
        TrainRunningNumberPkg number;
    };
//=======================================================================================
    /**
     * Сообщение 156 Termination of a communication session
     */
    struct TerminationCSMsg
    {
        TerminationCSMsg(const BrusHeader &header)
        : header(header)
        {}

        BrusHeader header;
    };
//=======================================================================================
    /**
     * Сообщение 157 Start Of Mission PositionReportPkg Report
     */
    struct StartOfMissionPrMsg
    {
        StartOfMissionPrMsg(const BrusHeader            &header,
                            const bool                  status,
                            const PositionReportPkg     &position,
                            const TrainRunningNumberPkg &number,
                            const std::shared_ptr<AppDataFromBrusPkg> &app_data)
        : header  (header),
          status  (status),
          position(position),
          number  (number),
          app_data(app_data)
        {}

        BrusHeader            header;
        bool                  status;   ///< Статус данных местоположения (Q_STATUS)
        PositionReportPkg     position;
        TrainRunningNumberPkg number;

        std::shared_ptr<AppDataFromBrusPkg> app_data;
    };
//=======================================================================================
    /**
     * Сообщение 159 Session established
     */
    struct SessionEstablishedMsg
    {
        SessionEstablishedMsg(const BrusHeader &header)
        : header(header)
        {}

        BrusHeader header;
    };
//=======================================================================================
    /**
     * Сообщение 160 Channels state - not from Subset-026
     */
    struct ChannelStateMsg
    {
        ChannelStateMsg(const BrusHeader &header,
                        const bool is_normal_alive,
                        const bool is_redundant_alive)
        : header            (header),
          is_normal_alive   (is_normal_alive),
          is_redundant_alive(is_redundant_alive)
        {}

        BrusHeader header;
        bool       is_normal_alive;
        bool       is_redundant_alive;
    };
//=======================================================================================
    /**
     * Сообщение 161 Actual Distance - not from Subset-026
     */
    struct ActualDistanceMsg
    {
        ActualDistanceMsg(const BrusHeader &header,
                          const uint16_t distance)
        : header  (header),
          distance(distance)
        {}

        BrusHeader header;
        uint16_t   distance;
    };
//================================== RBC->BRUS MESSAGES =================================
     /**
      * Сообщение 2 SR Authorisation
      */
     struct SrAuthMsg
     {
         SrAuthMsg(const RbcHeader &header,
                   const uint8_t    scale,
                   const uint16_t   distance,
                   const std::shared_ptr<ListOfBalisesInSrAuthPkg> &balises)
         : header  (header),
           scale   (scale),
           distance(distance),
           balises (balises)
         {}

         RbcHeader header;
         uint8_t    scale;    ///< Шкала расстояний, устанавливается в 1 (Q_SCALE)
         uint16_t   distance; ///< Расстояние в режиме SR (D_SR)

         std::shared_ptr<ListOfBalisesInSrAuthPkg> balises;
     };
//=======================================================================================
     /**
      * Сообщение 3 Movement Authority: Разрешение на движение
      */
     struct MovementAuthorityMsg
     {
         MovementAuthorityMsg(const RbcHeader             &header,
                              const LinkingPkg            &links,
                              const MovementAuthPkg       &movement_auth,
                              const std::shared_ptr<LvlTransitionOrderPkg>     &lvl_transition_order,
                              const std::shared_ptr<TemporarySpeedRestrPkg>    &speed_restr,
                              const std::shared_ptr<TemporarySpeedRestrRevPkg> &speed_restr_rev)
         : header              (header),
           links               (links),
           movement_auth       (movement_auth),
           lvl_transition_order(lvl_transition_order),
           speed_restr         (speed_restr),
           speed_restr_rev     (speed_restr_rev)
         {}

         RbcHeader       header;
         LinkingPkg      links;
         MovementAuthPkg movement_auth;

         std::shared_ptr<LvlTransitionOrderPkg>     lvl_transition_order;
         std::shared_ptr<TemporarySpeedRestrPkg>    speed_restr;
         std::shared_ptr<TemporarySpeedRestrRevPkg> speed_restr_rev;
     };
//=======================================================================================
    /**
     * Сообщение 8 AckMsg of Train ValidatedTrainDataPkg: Подтверждение данных поезда
     */
    struct AckOfTrainDataMsg
    {
        AckOfTrainDataMsg(const RbcHeader &header,
                          const uint32_t  atd_time)
        : header(header),
          time  (atd_time)
        {}

        RbcHeader header;
        uint32_t  time;  ///< Время отправки подтверждаемого сообщения с пакетом 11
    };
//=======================================================================================
    /**
     * Сообщение 16 Unconditional Emergency Stop: Безусловная аварийная остановка
     */
    struct UnconditionalESMsg
    {
        UnconditionalESMsg(const RbcHeader &header,
                           const uint8_t   es_id)
        : header(header),
          es_id (es_id)
        {}

        RbcHeader header;
        uint8_t    es_id; ///< Идентификатор сообщения аварийной остановки (NID_EM)
    };
//=======================================================================================
    /**
     * Сообщение 18 Revocation of Emergency Stop
     */
    struct RevocationOfESMsg
    {
        RevocationOfESMsg( const RbcHeader &header,
                           const uint8_t    es_id)
            : header(header),
              es_id (es_id)
        {}

        RbcHeader header;
        uint8_t   es_id; ///< Идентификатор сообщения аварийной остановки (NID_EM)
    };
//=======================================================================================
    /**
     * Сообщение 24 General message: Сообщение общего типа
     */
    struct GeneralMessage
    {
        GeneralMessage(const RbcHeader &header,
                       const std::shared_ptr<SessionManagementPkg> &session_management,
                       const std::shared_ptr<AppDataFromRbcPkg>    &app_data)
       : header            (header),
         session_management(session_management),
         app_data(app_data)
        {}

        RbcHeader header;

        std::shared_ptr<SessionManagementPkg> session_management;
        std::shared_ptr<AppDataFromRbcPkg>    app_data;
    };
//=======================================================================================
    /**
     * Сообщение 32 RBC/RIU System Version: Конфигурация РБЦ
     */
    struct SystemVersionMsg
    {
        SystemVersionMsg(const RbcHeader &header,
                         const uint8_t    version)
        : header (header),
          version(version)
        {}

        RbcHeader header;
        uint8_t    version; ///< Версия ETCS. Устанавливается в 010 0001 (M_VERSION)
    };
//=======================================================================================
    /**
     * Сообщение 39 AckMsg of termination of a communication session: Подтверждение завершения сеанса связи
     */
    struct AckOfTerminationCSMsg
    {
        AckOfTerminationCSMsg(const RbcHeader &header)
        : header(header)
        {}

        RbcHeader header;
    };
//=======================================================================================
    /**
     * Сообщение 40 Train Rejected: Поезд отклонен
     */
    struct TrainRejectedMsg
    {
        TrainRejectedMsg(const RbcHeader &header)
        : header(header)
        {}

        RbcHeader header;
    };
//=======================================================================================
    /**
     * Сообщение 41 Train Accepted: Поезд принят
     */
    struct TrainAcceptedMsg
    {
        TrainAcceptedMsg(const RbcHeader &header)
        : header(header)
        {}

        RbcHeader header;
    };
}

}

#endif //RBC_GW_MESSAGES_H