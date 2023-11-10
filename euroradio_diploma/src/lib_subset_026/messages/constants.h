#ifndef RBC_GW_CONSTANTS_H
#define RBC_GW_CONSTANTS_H

#include <cstdint>

namespace euroradio
{

namespace subset_026
{

// Not enum class on purpose
    enum VariableLength : std::size_t
    {
        D_ENDTIMERSTARTLOC    = 15, ///< Расстояние от точки запуска таймера последнего участка до точки окончания разрешения на движение
        D_LEVELTR             = 15, ///< Расстояние до изменения режима
        D_LINK                = 15, ///< Расстояние между группами бализ
        D_LRBG                = 15, ///< Расстояние от последней пройденной бализы
        D_DP                  = 15, ///< Расстояние от конца разрешения на движения до опасной точки
        D_OL                  = 15, ///< Расстояние от конца разрешения на движения до точки перекрытия
        D_SECTIONTIMERSTOPLOC = 15, ///< Расстояние от начала участка до точки остановки таймера участка
        D_STARTOL             = 15, ///< Расстояние от точки старта таймера перекрытия до точки окончания разрешения на движения
        D_SR                  = 15, ///< Расстояние в режиме SR
        D_TSR                 = 15, ///< Расстояние до точки начала действия временного ограничения скорости
        D_ACTUAL              = 15, ///< Актуальное расстояние (не часть Subset-026)
        L_ACKLEVELTR          = 15, ///< Длина области подтверждения режима
        L_DOUBTOVER           = 15, ///< Верхняя граница доверительного интервала
        L_DOUBTUNDER          = 15, ///< Нижняя граница доверительного интервала
        L_ENDSECTION          = 15, ///< Длина последнего участка в разрешении на движение
        L_MESSAGE             = 10, ///< Длина сообщения в байтах, включая все пакеты и переменные, включая заголовок
        L_PACKET              = 13, ///< Длина пакета в битах, включая все параметры и заголовок
        L_TRAIN               = 12, ///< Длина поезда в метрах
        L_TRAININT            = 15, ///< Безопасная длина поезда
        L_SECTION             = 15, ///< Длина участка в разрешении на движение
        L_TSR                 = 15, ///< Длина временного ограничения скорости
        M_ACK                 = 1,  ///< Определяет должно ли сообщение быть подтверждено или нет
        M_AIRTIGHT            = 2,  ///< Наличие системы герметичности
        M_AXLELOADCAT         = 7,  ///< Категория нагрузки на ось
        M_LEVEL               = 3,  ///< Уровень управления
        M_LEVELTR             = 3,  ///< Требуемый уровень управления
        M_LOADINGGAUGE        = 8,  ///< Профиль загрузки поезда
        M_MODE                = 4,  ///< Бортовой режим управления
        M_VERSION             = 7,  ///< Определяет тип системы тяги, используемый на линии (дизель, ...)
        M_VOLTAGE             = 4,  ///< Напряжение тяговой системы
        N_AXLE                = 10, ///< Количество осей локомотива
        N_ITER                = 5,  ///< Количество повторений данных следующих за этой переменной в пакете
        NC_CDTRAIN            = 4,  ///< Категория уклона поезда
        NC_TRAIN              = 15, ///< Категория поезда
        NID_BG                = 14, ///< Идентификатор группы бализ
        NID_C                 = 10, ///< Идентификатор зоны контроля РБЦ
        NID_CTRACTION         = 10, ///< Национальный идентификатор системы тяги
        NID_EM                = 4,  ///< Идентификатор сообщения аварийной остановки
        NID_ENGINE            = 24, ///< Уникальный ETCS идентификатор бортового устройства
        NID_LRBG              = 24, ///< Идентификатор последней пройденной бализы
        NID_MESSAGE           = 8,  ///< Идентификатор сообщения
        NID_NTC               = 8,  ///< Идентификатор национальной системы
        NID_OPERATIONAL       = 32, ///< Эксплуатационный номер поезда. Состоит из 8 цифр. Цифры записываются в порядке слева направо. В случае если номер поезда короче 8 цифр, остальные байты заполняются символом "F" (1111).
        NID_PACKET            = 8,  ///< Идентификатор пакета
        NID_RADIO             = 64, ///< Номер РБЦ
        NID_RBC               = 14, ///< Идентификатор РБЦ
        NID_TSR               = 8,  ///< Идентификатор временного ограничения скорости
        NID_XUSER             = 9,  ///< Идентификатор системы
        Q_DANGERPOINT         = 1,  ///< Флаг наличия данных опасной точки
        Q_DLRBG               = 2,  ///< Сторона бализы
        Q_DIR                 = 2,  ///< Направления достоверности передаваемых данных со ссылкой на направленность группы последних пройденных бализ
        Q_DIRLRBG             = 2,  ///< Ориентация поезда относительно направления последней пройденной бализы
        Q_DIRTRAIN            = 2,  ///< Направление движения поезда относительно направления последней пройденной бализы
        Q_EMERGENCYSTOP       = 2,  ///< Результат подтверждения аварийной остановки
        Q_ENDTIMER            = 1,  ///< Флаг наличия таймера для последнего участка в разрешении на движение
        Q_FRONT               = 1,  ///< Указывает действует ли ограничение скорости до начала поезда или до того как поезд полностью проедет участок
        Q_LENGTH              = 2,  ///< Целостность поезда
        Q_LOCACC              = 6,  ///< Точность местоположения бализы
        Q_LINKORIENTATION     = 1,  ///< Направление группы бализ
        Q_LINKREACTION        = 2,  ///< Действие на нарушение соединения бализ
        Q_MARQSTREASON        = 5,  ///< Причина запроса разрешения на движения
        Q_NEWCOUNTRY          = 1,  ///< Флаг определяющий относится ли новая бализа той же зоны относительно предыдущей в том же пакете (для первой бализы в пакете относительно предыдущего сообщения)
        Q_OVERLAP             = 1,  ///< Флаг наличия перекрытия
        Q_RBC                 = 1,  ///< Флаг управления сеансом связи
        Q_SCALE               = 2,  ///< Шкала расстояний
        Q_SECTIONTIMER        = 1,  ///< Флаг наличия таймера для участка
        Q_SLEEPSESSION        = 1,  ///< Флаг использования управления сессии
        Q_STATUS              = 2,  ///< Статус данных местоположения
        T_EMA                 = 10, ///< Время валидности целевой скорости в конце разрешения на движение
        T_ENDTIMER            = 10, ///< Время валидности последнего участка в разрешении на движение
        T_OL                  = 10, ///< Время, которое перекрытие может быть доступно, считая от времени достижения поездом точки D_STARTOL.
        T_TRAIN               = 32, ///< Временная метка отправки сообщения от времени запуска ПО
        T_SECTIONTIMER        = 10, ///< Время валидности участка в разрешении на движение
        V_EMA                 = 7,  ///< Разрешенная скорость в конце разрешения на движение
        V_MAXTRAIN            = 7,  ///< Максимально допустимая скорость поезда
        V_RELEASEDP           = 7,  ///< Скорость опасной точки
        V_RELEASEOL           = 7,  ///< Скорость перекрытия
        V_TRAIN               = 7,  ///< Скорость поезда
        V_TSR                 = 7,  ///< Разрешенная скорость временного ограничения скорости
    };

    enum AppDataVarLength : std::size_t
    {
        ID_AREA    = 32,
        NID_BALISE = 16,
        DISTANCE   = 16,
        CRC_32     = 32,
        MAP_VER    = 32
    };

    enum ChannelStateVarLength : std::size_t
    {
        Q_NORMAL    = 1,
        Q_REDUNDANT = 1
    };

    enum class MessageID : uint8_t //8bit value
    {
        //====== RBC -> BRUS ======
        SR_AUTHORISATION      = 2,
        MOVEMENT_AUTHORITY    = 3,
        ACK_TRAIN_DATA        = 8,
        UNCONDITIONAL_ES      = 16,
        REVOCATION_OF_ES      = 18,
        GENERAL_MSG           = 24,
        SYS_VERSION           = 32,
        ACK_TERMINATION       = 39,
        REJECTED              = 40,
        ACCEPTED              = 41,
        //====== BRUS -> RBC ======
        VALIDATED_TRAIN_DATA  = 129,
        MA_REQUEST            = 132,
        TRAIN_POSITION_REPORT = 136,
        ACKNOWLEDGEMENT       = 146,
        ES_ACKNOWLEDGEMENT    = 147,
        END_OF_MISSION        = 150,
        NO_COMPATIBLE_VERSION = 154,
        CS_INITIATION         = 155, //CS - Communication Session
        CS_TERMINATION        = 156,
        START_OF_MISSION_PR   = 157,
        CS_ESTABLISHED        = 159,
        CHANNEL_STATE         = 160,
        ACTUAL_DISTANCE       = 161
    };

    inline std::ostream& operator<<(std::ostream& os, const MessageID &id)
    {
        switch (id)
        {
            case MessageID::SR_AUTHORISATION:
                return os << "SrAuthMsg";
            case MessageID::MOVEMENT_AUTHORITY:
                return os << "MovementAuthorityMsg";
            case MessageID::ACK_TRAIN_DATA:
                return os << "AckOfTrainDataMsg";
            case MessageID::UNCONDITIONAL_ES:
                return os << "UnconditionalESMsg";
            case MessageID::REVOCATION_OF_ES:
                return os << "RevocationOfESMsg";
            case MessageID::GENERAL_MSG:
                return os << "GeneralMessage";
            case MessageID::SYS_VERSION:
                return os << "SystemVersionMsg";
            case MessageID::ACK_TERMINATION:
                return os << "AckOfTerminationCSMsg";
            case MessageID::REJECTED:
                return os << "TrainRejectedMsg";
            case MessageID::ACCEPTED:
                return os << "TrainAcceptedMsg";
            case MessageID::VALIDATED_TRAIN_DATA:
                return os << "ValidatedTrainDataMsg";
            case MessageID::MA_REQUEST:
                return os << "MaRequestMsg";
            case MessageID::TRAIN_POSITION_REPORT:
                return os << "TrainPositionReportMsg";
            case MessageID::ACKNOWLEDGEMENT:
                return os << "AckMsg";
            case MessageID::ES_ACKNOWLEDGEMENT:
                return os <<  "SrAuthMsg";
            case MessageID::END_OF_MISSION:
                return os << "EndOfMissionMsg";
            case MessageID::NO_COMPATIBLE_VERSION:
                return os << "SrAuthMsg";
            case MessageID::CS_INITIATION:
                return os << "InitiationCSMsg";
            case MessageID::CS_TERMINATION:
                return os << "TerminationCSMsg";
            case MessageID::START_OF_MISSION_PR:
                return os << "StartOfMissionPrMsg";
            case MessageID::CS_ESTABLISHED:
                return os << "SessionEstablishedMsg";
            default:
                return os << "\"Wrong id\"";
        }
    }

    inline bool isRbcMessage( const MessageID &id )
    {
        return (static_cast<uint8_t>(id) <= 41); ///< Messages with id > 41 is BRUS messages
    }

    /**
     * Message length in bits
     */
    enum class MessageLength : uint16_t
    {
        RBC_HEADER            = 75,  ///< 10 bytes
        BRUS_HEADER           = 74,  ///< 10 bytes
        //====== RBC -> BRUS ======
        SR_AUTHORISATION      = 92,  ///< 12 bytes
        MOVEMENT_AUTHORITY    = 75,  ///< 10 bytes
        ACK_TRAIN_DATA        = 107, ///< 14 bytes
        UNCONDITIONAL_ES      = 79,  ///< 10 bytes
        REVOCATION_OF_ES      = 79,  ///< 10 bytes
        GENERAL_MSG           = 75,  ///< 10 bytes
        SYS_VERSION           = 82,  ///< 11 bytes
        ACK_TERMINATION       = 75,  ///< 10 bytes
        REJECTED              = 75,  ///< 10 bytes
        ACCEPTED              = 75,  ///< 10 bytes
        //====== BRUS -> RBC ======
        VALIDATED_TRAIN_DATA  = 264, ///< 33 bytes
        MA_REQUEST            = 216, ///< 27 bytes
        TRAIN_POSITION_REPORT = 264, ///< 33 bytes
        ACKNOWLEDGEMENT       = 106, ///< 14 bytes
        ES_ACKNOWLEDGEMENT    = 217, ///< 28 bytes
        END_OF_MISSION        = 211, ///< 27 bytes
        NO_COMPATIBLE_VERSION = 74,  ///< 10 bytes
        CS_INITIATION         = 127, ///< 16 bytes
        CS_TERMINATION        = 74,  ///< 10 bytes
        START_OF_MISSION_PR   = 266, ///< 34 bytes
        CS_ESTABLISHED        = 74,  ///< 10 bytes
        CHANNEL_STATE         = 76,  ///< 10 bytes
        ACTUAL_DISTANCE       = 89   ///< 12 bytes
    };

    enum class PackageID : uint8_t // 8bit value
    {
        POSITION_REPORT      = 0,
        TRAIN_RUNNING_NUMBER = 5,
        VALIDATED_TRAIN_DATA = 11,
        LINKING              = 5,
        MOVEMENT_AUTH        = 15,
        LVL_TRANSITION_ORDER = 41,
        SESSION_MNG          = 42,
        RBC_APP_DATA         = 44,
        BRUS_APP_DATA        = 44,
        LIST_OF_BALISES      = 63,
        SPEED_RESTR          = 65,
        SPEED_RESTR_REV      = 66
    };

    /**
     * Package length in bits
     */
    enum class PackageLength : uint16_t
    {
        BRUS_HEADER          = 21,   ///< 3  bytes
        RBC_HEADER           = 23,   ///< 3  bytes
        POSITION_REPORT      = 137,  ///< 18 bytes
        TRAIN_RUNNING_NUMBER = 53,   ///< 7  bytes
        VALIDATED_TRAIN_DATA = 96,   ///< 12 bytes
        LINKING              = 79,   ///< 10 bytes
        MOVEMENT_AUTH        = 185,  ///< 24 bytes
        LVL_TRANSITION_ORDER = 71,   ///< 9  bytes
        SESSION_MNG          = 113,  ///< 15 bytes
        RBC_APP_DATA         = 128,  ///< 16 bytes
        BRUS_APP_DATA        = 64,   ///< 8  bytes
        LIST_OF_BALISES      = 28,   ///< 4  bytes
        SPEED_RESTR          = 71,   ///< 9  bytes
        SPEED_RESTR_REV      = 31    ///< 4  bytes
    };

}

}

#endif //RBC_GW_CONSTANTS_H