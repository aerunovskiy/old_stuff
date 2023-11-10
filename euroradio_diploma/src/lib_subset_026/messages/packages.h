#ifndef RBC_GW_PACKAGES_H
#define RBC_GW_PACKAGES_H

#include <vector>
#include <iostream>

namespace euroradio
{

namespace subset_026
{
//================================== BRUS->RBC PACKAGES =================================
    /**
     * Пакет 0 PositionReportPkg Report
     */
    struct PositionReportPkg
    {
        PositionReportPkg(const uint8_t  scale,
                          const uint32_t last_balise_id,
                          const uint16_t distance_passed,
                          const uint8_t  balise_orientation,
                          const uint8_t  balise_side,
                          const uint16_t upper_confid_int,
                          const uint16_t lower_confid_int,
                          const bool     train_integrity,
                          const uint16_t length_in_cars,
                          const uint8_t  speed,
                          const bool     is_nominal_direction,
                          const uint8_t  control_mode,
                          const uint8_t  control_lvl,
                          const uint8_t  nid)
        : scale               (scale),
          last_balise_id      (last_balise_id),
          distance_passed     (distance_passed),
          balise_orientation  (balise_orientation),
          balise_side         (balise_side),
          upper_confid_int    (upper_confid_int),
          lower_confid_int    (lower_confid_int),
          train_integrity     (train_integrity),
          length_in_cars      (length_in_cars),
          speed               (speed),
          is_nominal_direction(is_nominal_direction),
          control_mode        (control_mode),
          control_lvl         (control_lvl),
          nid                 (nid)
          {}

        uint8_t  scale;                 ///< Шкала расстояний (Q_SCALE)
        uint32_t last_balise_id;	    ///< ID последней пройденной виртуальной бализы (NID_LRBG)
        uint16_t distance_passed;       ///< Пройденное головой поезда расстояние от последней бализы (D_LRBG)
        uint8_t  balise_orientation;    ///< Ориентация поезда относительно направления последней пройденной
                                        ///< бализы (Q_DIRLRBG)
        uint8_t  balise_side;           ///< Сторона бализы. Устанавливается в 1 (Q_DLRBG)
        uint16_t upper_confid_int;      ///< Верхний доверительный интервал (L_DOUBTOVER)
        uint16_t lower_confid_int;      ///< Нижиний доверительный интервал (L_DOUBTUNDER)
        bool     train_integrity;       ///< Целостность поезда (Q_LENGTH)
        uint16_t length_in_cars;        ///< Безопасная длина поезда. Если целостность поезда подтверждена,
                                        ///< иначе 0 (L_TRAININT)
        uint8_t  speed; 	            ///< Скорость поезда (V_TRAIN)
        bool     is_nominal_direction;  ///< Направление движения поезда относительно направления последней
                                        ///< пройденной бализы (Q_DIRTRAIN)
        uint8_t  control_mode;          ///< Бортовой режим управления (M_MODE)
        uint8_t  control_lvl;           ///< Уровень управления (M_LEVEL)
        uint8_t  nid;                   ///< Идентификатор национальной системы. Устанавливается в 0
                                        ///< Если M_LEVEL = 1 (NID_NTC)
    };
//=======================================================================================
    /**
     * Пакет 5 Train running number
     */
        struct TrainRunningNumberPkg
        {
            TrainRunningNumberPkg(const  uint32_t number)
            : number(number)
            {}

             uint32_t number; ///< Номер поезда (NID_OPERATIONAL)
        };

        struct Axle
        {
            uint8_t voltage;            ///< Напряжение тяговой системы. Устанавливается в 3 (M_VOLTAGE)
            uint8_t nid ;               ///< Национальный идентификатор системы тяги. Устанавливается в 1 (NID_CTRACTION)
        };
//=======================================================================================
    /**
     * Пакет 11 Validated train data
     */
    struct ValidatedTrainDataPkg
    {
        ValidatedTrainDataPkg(const uint8_t  train_category,
                              const uint16_t train_type,
                              const uint16_t length_in_cars,
                              const uint8_t  max_speed,
                              const uint8_t  loading_prof,
                              const uint8_t  axis_load,
                              const bool     is_airtight,
                              const uint16_t axis_number,
                              const std::vector<Axle>   &axis,
                              const std::vector<int8_t> &nids)
        : train_category(train_category),
          train_type    (train_type),
          length_in_cars(length_in_cars),
          max_speed     (max_speed),
          loading_prof  (loading_prof),
          axis_load     (axis_load),
          is_airtight   (is_airtight),
          axis_number   (axis_number),
          axis          (axis),
          nids          (nids)
        {}

        uint8_t  train_category; ///< Категория поезда. Устанавливается в 15 (NC_CDTRAIN)
        uint16_t train_type;     ///< Категория поезда (NC_TRAIN)
        uint16_t length_in_cars; ///< Длина поезда в вагонах (L_TRAIN в метрах)
        uint8_t  max_speed;      ///< Максимальная скорость поезда. Устанавливается в 32 (V_MAXTRAIN)
        uint8_t  loading_prof;   ///< Профиль загрузки поезда. Устанавливается в 0 (M_LOADINGGAUGE)
        uint8_t  axis_load;      ///< Категория нагрузки на ось. Устанавливается в 2 (M_AXLELOADCAT)
        bool     is_airtight;    ///< Наличие системы герметичности. Устанавливается в 0 (M_AIRTIGHT)
        uint16_t axis_number;    ///< Количество осей (N_AXLE)

        std::vector<Axle>   axis;
        std::vector<int8_t> nids;  ///< Идентификатор национальной системы. Устанавливается в 0 (NID_NTC)
    };
//=======================================================================================
    /**
    * Пакет 44 Data used by applications outside the ERTMS/ETCS system
    * @brief Используется для передачи данных для приложений
    */
    struct AppDataPkg
    {
        explicit AppDataPkg(const uint16_t system_id)
        : system_id(system_id)
        {}

        uint16_t system_id; ///< Идентификатор системы (NID_XUSER)
    };
//=======================================================================================
    struct AppDataFromBrusPkg : AppDataPkg
    {
        AppDataFromBrusPkg(const uint16_t system_id,
                           const uint32_t map_version)
        : AppDataPkg(system_id),
          map_version(map_version)
        {}

        uint32_t map_version;
    };
//================================== BRUS->RBC PACKAGES =================================
    /**
     * Структура для каждой группы бализ
     */
    struct BaliseGroup
    {
        uint16_t balise_dist;              ///< Расстояние между группами бализ (D_LINK)
        bool     new_country;              ///< Флаг изменения зоны (Q_NEWCOUNTRY)
        uint16_t zone;                     ///< Идентификатор зоны контроля РБЦ. Если Q_NEWCOUNTRY = 1 (NID_C)
        uint16_t balise_group_id;          ///< Идентификатор группы бализ (NID_BG)
        bool     balise_group_orientation; ///< Направление группы бализ (Q_LINKORIENTATION)
        uint8_t  on_link_error;            ///< Действие при нарушении соединения бализ (Q_LINKREACTION)
        uint8_t  location_accuracy;        ///< Точность местоположения бализы (Q_LOCACC)
    };
//=======================================================================================
    /**
      * Пакет 5 Linking
      * @brief Используется для передачи всех виртуальных бализ в разрешении на движение.
      */
    struct LinkingPkg
    {
        LinkingPkg(const uint8_t     scale,
                   const BaliseGroup &last,
                   const std::vector<BaliseGroup> &balises)
        : scale  (scale),
          last   (last),
          balises(balises)
        {}

        uint8_t     scale; ///< Шкала расстояний, устанавливается в 1 (	Q_SCALE)
        BaliseGroup last;

        std::vector<BaliseGroup> balises;
    };
//=======================================================================================
    /**
     * Информаци о секции
     */
    struct Section
    {
        uint16_t length;               ///< Длина участка в разрешении на движение (L_SECTION)
        bool     timer;                ///< Флаг наличия таймера для участка (Q_SECTIONTIMER)
        uint16_t zone_expiration_time; ///< Время валидности участка в разрешении на движение (T_SECTIONTIMER)
        uint16_t length_to_stop;       ///< Расстояние от начала участка до точки остановки таймера участка
                                       ///< (D_SECTIONTIMERSTOPLOC)
    };
//=======================================================================================
    /**
      * Пакет 15 Level 2/3 Movement Authority
      * @brief Используется для передачи разрешения на движение
      */
    struct MovementAuthPkg
    {
        MovementAuthPkg(const uint8_t  scale,
                        const uint8_t  allowed_speed_end,
                        const uint16_t speed_expire_time,
                        const std::vector<Section> &sections,
                        const Section  &last,
                        const bool     is_timer,
                        const uint16_t zone_expiration_time,
                        const uint16_t timer_start_distance,
                        const bool     is_danger_point,
                        const uint16_t distance_from_auth,
                        const uint8_t  danger_speed,
                        const bool     is_overlap,
                        const uint16_t distance_from_start,
                        const uint16_t overlap_time,
                        const uint16_t distance_from_end,
                        const uint8_t  overlap_speed)
        : scale               (scale),
          allowed_speed_end   (allowed_speed_end),
          speed_expire_time   (speed_expire_time),
          sections            (sections),
          last                (last),
          is_timer            (is_timer),
          zone_expiration_time(zone_expiration_time),
          timer_start_distance(timer_start_distance),
          is_danger_point     (is_danger_point),
          distance_from_auth  (distance_from_auth),
          danger_speed        (danger_speed),
          is_overlap          (is_overlap),
          distance_from_start (distance_from_start),
          overlap_time        (overlap_time),
          distance_from_end   (distance_from_end),
          overlap_speed       (overlap_speed)
        {}

        uint8_t  scale;             ///< Шкала расстояний, устанавливается в 1 (Q_SCALE)
        uint8_t  allowed_speed_end; ///< Разрешенная скорость в конце разрешения на движение (V_EMA)
        uint16_t speed_expire_time; ///< Время валидности целевой скорости в конце разрешения на движение (T_EMA)

        std::vector<Section> sections;

        Section last; ///< Последняя зона (но это не точно)

        bool is_timer; ///< Флаг наличия таймера для последнего участка в разрешении на движение (Q_ENDTIMER)
            uint16_t zone_expiration_time; ///< Время валидности последнего участка в разрешении на движение
                                           ///< (T_ENDTIMER )
            uint16_t timer_start_distance; ///< Расстояние от точки запуска таймера последнего участка до точки
                                           ///< окончания разрешения на движение (D_ENDTIMERSTARTLOC )

        bool is_danger_point; ///< Флаг наличия данных опасной точки. Устанавливается в 0 (Q_DANGERPOINT)
            uint16_t distance_from_auth; ///< Расстояние от конца разрешения на движения до опасной точки (D_DP)
            uint8_t  danger_speed;       ///< Скорость опасной точки (V_RELEASEDP)

        bool is_overlap; ///< Флаг наличия перекрытия. Устанавливается в 0 (Q_OVERLAP)
            uint16_t distance_from_start; ///< Расстояние от точки старта таймера перекрытия до точки окончания
                                          ///< разрешения на движения (D_STARTOL )
            uint16_t overlap_time;        ///< Время действия перекрытия (T_OL)
            uint16_t distance_from_end;   ///< Расстояние от конца разрешения на движения до точки перекрытия (D_OL)
            uint8_t  overlap_speed;       ///< Скорость перекрытия (V_RELEASEOL)
    };
//=======================================================================================
    /**
      * Указ
      */
    struct Order
    {
        uint16_t order_lvl;    ///< Требуемый уровень управления (M_LEVELTR)
        uint8_t  nid;          ///< Идентификатор национальной системы. Устанавливается в 0
                               ///< Если M_LEVELTR = 1 (NID_NTC)
        uint16_t ack_distance; ///< Длина области подтверждения режима (L_ACKLEVELTR)
    };
//=======================================================================================
    /**
      * Пакет 41 Level Transition Order
      * @brief Используется для изменения уровня управления
      */
    struct LvlTransitionOrderPkg
    {
        LvlTransitionOrderPkg(const uint8_t            scale,
                              const uint16_t           change_distance,
                              const Order              &current,
                              const std::vector<Order> &orders)
        : scale          (scale),
          change_distance(change_distance),
          current        (current),
          orders         (orders)
        {}

        uint8_t  scale; ///< Шкала расстояний, устанавливается в 1 (Q_SCALE)

        uint16_t change_distance; ///< Расстояние до изменения режима (D_LEVELTR)
        Order    current;         ///< Последний (наверное)

        std::vector<Order> orders;
    };
//=======================================================================================
    /**
      * Пакет 42 Session Management
      * @brief Используется для управления установки или завершения сеанса связи с РБЦ
      */
    struct SessionManagementPkg
    {
        SessionManagementPkg(const bool     control_flag,
                             const uint16_t zone_id,
                             const uint16_t rbc_id,
                             const uint64_t rbc_num,
                             const bool     is_alive)
        : control_flag(control_flag),
          zone_id     (zone_id),
          rbc_id      (rbc_id),
          rbc_num     (rbc_num),
          is_alive    (is_alive)
        {}

        bool     control_flag; ///< Флаг управления сеансом связи (Q_RBC)
        uint16_t zone_id;      ///< Идентификатор зоны (NID_C)
        uint16_t rbc_id;       ///< Идентификатор РБЦ. Устанавливается в 16383 (NID_RBC)
        uint64_t rbc_num;      ///< Номер РБЦ (NID_RADIO)
        bool     is_alive;     ///< Флаг использования управления сессии. Устанавливается в 1 (Q_SLEEPSESSION)
    };
//=======================================================================================
    /**
      * Пакет 44 Data used by applications outside the ERTMS/ETCS system
      * @brief Используется для передачи данных для приложений
      */
    struct AppDataFromRbcPkg : AppDataPkg
    {
        AppDataFromRbcPkg(const uint16_t system_id,
                          const uint32_t zone,
                          const uint16_t balise_id,
                          const uint16_t distance,
                          const uint32_t crc_32)
        : AppDataPkg(system_id),
          zone      (zone),
          balise_id (balise_id),
          distance  (distance),
          crc_32    (crc_32)
        {}

        uint32_t zone;
        uint16_t balise_id;
        uint16_t distance;
        uint32_t crc_32;
    };
//=======================================================================================
    struct Balise
    {
        bool new_zone;               ///< Флаг новой зоны (Q_NEWCOUNTRY)
            uint16_t zone_id;         ///< Идентификатор зоны / региона (NID_C)
            uint16_t balise_group_id; ///< Идентификатор группы бализ (NID_BG)
    };
//=======================================================================================
    /**
     * Пакет 63 List of Balises in SR Authorisation
     * @brief Используется для составления списка группы (групп) бализ, которые поезд может проезжать в режиме SR
     */
    struct ListOfBalisesInSrAuthPkg
    {
        ListOfBalisesInSrAuthPkg(const std::vector<Balise> &balises) : balises(balises){}

        std::vector<Balise> balises;
    };
//=======================================================================================
    /**
      * Пакет 65 Temporary Speed Restriction
      * @brief Используется для передачи временных ограничений скорости.
      */
    struct TemporarySpeedRestrPkg
    {
        TemporarySpeedRestrPkg(const uint8_t  scale,
                               const uint8_t  restr_id,
                               const uint16_t enable_dist,
                               const uint16_t active_dist,
                               const bool     is_valid,
                               const uint8_t  allowed_speed)
        : scale        (scale),
          restr_id     (restr_id),
          enable_dist  (enable_dist),
          active_dist  (active_dist),
          is_valid     (is_valid),
          allowed_speed(allowed_speed)
        {}

        uint8_t scale; ///< Шкала расстояний, устанавливается в 1 (Q_SCALE)

        uint8_t  restr_id;      ///< Идентификатор временного ограничения скорости (NID_TSR)
        uint16_t enable_dist;   ///< Расстояние до точки начала действия временного ограничения скорости (D_TSR)
        uint16_t active_dist;   ///< Длина временного ограничения скорости (L_TSR)
        bool     is_valid;      ///< Флаг валидности точки окончания профиля скорости (Q_FRONT)
        uint8_t  allowed_speed; ///< Разрешенная скорость временного ограничения скорости (V_TSR)
    };
//=======================================================================================
    /**
      * Пакет 66 Temporary Speed Restriction Revocation
      * @brief Используется для отмены временных ограничений скорости
      */
    struct TemporarySpeedRestrRevPkg
    {
        TemporarySpeedRestrRevPkg(const uint8_t restr_id)
        : restr_id(restr_id)
        {}

        uint8_t restr_id; ///< Идентификатор временного ограничения скорости (NID_TSR)
    };
}

}

#endif //RBC_GW_PACKAGES_H