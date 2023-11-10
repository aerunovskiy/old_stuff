#pragma once

#include "ale.h"
#include "vudpsocket.h"
#include "vtimer.h"
#include <queue>
#include <vsettings.h>

// Класс ale_controller декорирует взаимодействие одновременно с двумя слоями Adaptation Layer Entity
class AleController : public BasicAle
{
public:

    enum class RequestType
    {
        ConnectRequest,
        ConnectResponse,
        DisconnectRequest,
        DataRequest,
        TsnRequest,
        KeepAliveRequest
    };

    enum class ResponseType
    {
        ConnectIndication,
        ConnectConfirmation,
        DisconnectIndication,
        DataIndication,
        TsnIndication
    };

    template <typename E>
    static constexpr typename std::underlying_type<E>::type toUtype(E enumerator)
    {
        return static_cast<typename std::underlying_type<E>::type>(enumerator);
    }

public:

    AleController( const vsettings &settings );

    // Запрос на установление соединения
    void connectRequest      (  uint32_t            address_type,
                                const std::string&  network_address,
                                uint32_t            called_etcsid,
                                uint32_t            calling_etcsid,
                                uint8_t             application_type,
                                uint8_t             q_of_s,
                                const std::string&  user_data           ) override;

    // Ответ на запрос на установление соединения
    void connectResponse     (  uint16_t            tcepida,
                                uint32_t            responding_etcsid,
                                const std::string&  user_data           ) override;

    // Запрос на разрыв соединения
    void disconnectRequest   (  uint16_t            tcepidxmt,
                                const std::string&  user_data   ) override;

    // Запрос на передачу данных
    void dataRequest         (  uint16_t            tcepidxmt,
                                const std::string&  user_data   ) override;

    const std::string& getNetworkAddress(uint32_t responding_etcsid ) override;

private:
    void onAleConnectIndication    ( const ConnectIndicationData    &data );
    void onAleConnectConfirmation  ( const ConnectConfirmationData  &data );
    void onAleDisconnectIndication ( const DisconnectIndicationData &data );
    void onAleDataIndication       ( const DataIndicationData       &data );

    // Реакция на отправку сообщений слоем ALE
    void onAleMessageSending       ( uint16_t tcepid, uint16_t tsn );

     // Парсинг и выполнение отклика от удаленного ALE_controller в активном режиме
    void parseRemoteResponse( const std::string& data );

    // Парсинг и выполнение запроса от удаленного ALE_controller в пассивном режиме
    void parseRemoteRequest( const std::string& data );

    // Выполнение запросов от слоя SL, пришедших во время инициализации
    void doDeferredRequests();

private:

    // Базоввый слой ALE
    std::unique_ptr<Ale> _ale;

    // Сокет для связи с удаленным ALE_controller (второй полукомплект)
    VUdpSocket _remote_socket;
    SocketPair::vsocket_address _remote_address;

    // Master или Slave
    bool _is_primary;

    // Активный или пассивный
    bool _is_active;

    // Инициализированный
    bool _is_initialized;

    // Флаг блокировки DisconnectIndication
    bool _block_di;

    // Признаки приема ответов от ALE
    bool _host_received_flag;
    bool _remote_received_flag;

    bool _master_is_alive;

    // Данные о последних отправленных TSN по всем TCEPID
    std::map<uint16_t, uint16_t> _tsn_book;

    // Таймеры инициализации и обмена с удаленным ALE_controller
    VTimer _alive_timer;
    VTimer _init_timer;

    // Хранилище принятых от SL ConnectRequest-ов, не отработанных во время инициализации
    std::queue<ConnectRequestData> _connect_request_storage;

};
