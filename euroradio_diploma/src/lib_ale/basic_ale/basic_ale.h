#pragma once

#include <string>
#include "vsignal.h"

class BasicAle
{
public:

    static const int max_num_of_server_connections  = 100;  // Максимум серверных пар

    //====== ФОРМАТ ПАКЕТОВ ДЛЯ ПЕРЕДАЧИ В ПАРАМЕТРАХ СООТВЕТСТВУЮЩИХ СИГНАЛОВ ===============

        struct ConnectIndicationData
        {
            uint16_t    tcepida;            // TCEPID подключаемой пары сокетов
            std::string network_address;    // Сетевой адрес подключающейся к паре сущности
            uint32_t    calling_etcsid;     // ETCSID вызывающей сущности
            uint8_t     application_type;   // Тип приложения подключающейся к паре сущности
            uint8_t     q_of_s;             // Quality of Service (Class of Service)
            std::string user_data;          // AU1 SaPDU
        };

        struct ConnectConfirmationData
        {
            uint16_t    tcepidb;            // TCEPID, для которого подтверждено соединение
            uint32_t    responding_etcsid;  // ETCSID сущности, отвечающей на запрос коннекта
            std::string user_data;           // AU2 SaPDU
        };

        struct DisconnectIndicationData
        {
            DisconnectIndicationData(   uint16_t    tcepidrcv,
                                        uint16_t    reason,
                                        uint16_t    sub_reason,
                                        const std::string &user_data
                                    ):
                tcepidrcv(tcepidrcv),
                reason(reason),
                sub_reason(sub_reason),
                user_data(user_data)
            {}

            DisconnectIndicationData(   uint16_t    tcepidrcv,
                                        uint16_t    reason,
                                        uint16_t    sub_reason
                                    ):
                tcepidrcv(tcepidrcv),
                reason(reason),
                sub_reason(sub_reason)
            {}

            uint16_t    tcepidrcv;  // TCEPID пары, в которой произошел разрыв соединения
            uint16_t    reason;     // Причина разрыва соединения
            uint16_t    sub_reason; // Подпричина разрыва соединения
            std::string user_data;  // DI SaPDU (или пустая строка, если разрыв на уровне ALE)
        };

        struct DataIndicationData
        {
            uint16_t    tcepidrcv; // TCEPID пары, в которую пришел пакет данных
            std::string user_data;
            bool        channel;
        };

        struct ConnectRequestData
        {
            ConnectRequestData(     uint32_t    address_type,
                                    std::string network_address,
                                    uint32_t    called_etcsid,
                                    uint32_t    calling_etcsid,
                                    uint8_t     application_type,
                                    uint8_t     q_of_s,
                                    std::string user_data):
                address_type(address_type),
                network_address(network_address),
                called_etcsid(called_etcsid),
                calling_etcsid(calling_etcsid),
                application_type(application_type),
                q_of_s(q_of_s),
                user_data(user_data)
            {}

            uint32_t    address_type;
            std::string network_address;
            uint32_t    called_etcsid;
            uint32_t    calling_etcsid;
            uint8_t     application_type;
            uint8_t     q_of_s;
            std::string user_data;
        };

        //=================================== СИГНАЛЫ ============================================

            // Сигнал "Запрошено соединение"
            VSignal<ConnectIndicationData>    connect_indication;

            // Сигнал "Получено разрешение"
            VSignal<ConnectConfirmationData>  connect_confirmation;

            // Сигнал "Получены данные"
            VSignal<DataIndicationData>       data_indication;

            // Сигнал "Соединение разорвано"
            VSignal<DisconnectIndicationData> disconnect_indication;

            // Сигнал с сообщением о последнем отправленном TSN
            VSignal <uint16_t, uint16_t> report_tsn;

            // Сигнал об успешной отправке данных по каналу
            VSignal<bool> active_channel;
            //========================================================================================

        virtual ~BasicAle();

        // Запрос на установление соединения
        virtual void connectRequest      (  uint32_t    address_type,
                                    const std::string& network_address,
                                    uint32_t    called_etcsid,
                                    uint32_t    calling_etcsid,
                                    uint8_t     application_type,
                                    uint8_t     q_of_s,
                                    const std::string& user_data ) = 0;

        // Ответ на запрос на установление соединения
        virtual void connectResponse     (  uint16_t    tcepida,
                                    uint32_t    responding_etcsid,
                                    const std::string& user_data ) = 0;

        // Запрос на разрыв соединения
        virtual void disconnectRequest   (  uint16_t    tcepidxmt,
                                    const std::string& user_data ) = 0;

        // Запрос на передачу данных
        virtual void dataRequest         (  uint16_t    tcepidxmt,
                                    const std::string& user_data ) = 0;

        virtual const std::string& getNetworkAddress(uint32_t responding_etcsid) = 0;

    //========================================================================================
};
