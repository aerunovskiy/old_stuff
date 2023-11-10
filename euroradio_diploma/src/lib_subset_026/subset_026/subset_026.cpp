//=======================================================================================
#include "subset_026.h"

namespace euroradio
{
namespace subset_026
{

Subset_026::Subset_026(const vsettings   &settings,
                       MediatorInterface &interface)
    : _interface(interface)
{
    loadSettings(settings);

    _sl->connect_indication      .connect(this, &Subset_026::onConnectIndication);
    _sl->connect_confirmation    .connect(this, &Subset_026::onConnectConfirmation);
    _sl->disconnect_indication   .connect(this, &Subset_026::onDisconnectIndication);
    _sl->data_indication         .connect(this, &Subset_026::onDataIndication);
    _sl->ale_connect_indication  .connect(this, &Subset_026::onALEConnectIndication);
    _sl->ale_connect_confirmation.connect(this, &Subset_026::onALEConnectConfirmation);

    _sl->active_channel               .connect(_channel_state.get(), &ChannelState::resetChannel);
    _channel_state->send_channel_state.connect(this,                 &Subset_026  ::sendChannelStateMsg);

    _start_time = VTimePoint::now();
}
//=======================================================================================
void Subset_026::loadSettings(const vsettings &settings)
{
    vdeb << "Subset-026: start settings loading...";

    if (settings.subgroups().empty())
        verror("Subset-026: wrong config path!");

    auto _settings = settings.subgroup("Subset-026");

    try
    {
        _rbc_id           = _settings.subgroup("RBC")      .get<int32_t>("etcsid");
        auto wait_time    = _settings.subgroup("Intervals").get<int>    ("connect_response");
        _allowed_diff     = _settings.subgroup("Intervals").get<int>    ("allowed_diff");
        auto cs_wait      = _settings.subgroup("Intervals").get<int>    ("cs_wait");
        auto cs_interval  = _settings.subgroup("Intervals").get<int>    ("cs_interval");
        _sl               = std::make_shared<Sl>             (settings);
        _channel_state    = std::make_shared<ChannelState>   (cs_wait, cs_interval);
        _connection_table = std::make_shared<ConnectionTable>(wait_time, *this);

        auto log_settings = settings.subgroup("RbcGwController").subgroup("LogSettings");
        auto log_path     = log_settings.get ("path"    );

        _lag_control_on = _settings.subgroup("LagController").get<bool>("lag_control_on");
        if (_lag_control_on)
            _lag_controller = std::make_shared<LagController>(log_path);
    }
    catch (...)
    {
        throw verror("Subset-026: settings loading error!");
    }

    vdeb << "Subset-026: settings load successful!";
}
//=======================================================================================
void Subset_026::onConnectIndication(const uint16_t tcepid)
{
    vdeb << "Subset_026: Received signal Connect Indication from SL";
    _sl->connectResponse(tcepid);
}
//=======================================================================================
void Subset_026::onConnectConfirmation(const uint16_t tcepid)
{
    if (!_connection_table->getNoCsInit())
        vdeb << "Subset_026: Received signal Connect Confirmation from SL";

    auto params = _connection_table->getParametersByTcepid(tcepid);
    if (params != nullptr && _connection_table->confirmConnection(tcepid))
    {
        vdeb << "Subset_026: Sending Data Request (InitiationCSMsg) to ETCSID" << params->peer_etcsid;
        auto cs_init_msg = _connection_table->getPendingCsInitMsg(params->peer_etcsid);
        dataRequest(params->brus_id, to_bin::formInitiationCSMsg(*cs_init_msg).DataInString());

        _connection_table->removePendingCsInitMsg(params->peer_etcsid);

        connect_indication(tcepid);
    }
    else
    {
        vrunlog << "Subset_026 (onConnectConfirmation, last) : No actual params for established connection with TCEPID"
                << tcepid << ". Disconnecting.";
        disconnectRequest(tcepid, 0, 0);
        return;
    }
}
//=======================================================================================
void Subset_026::onDisconnectIndication(const Sl::DisRepIndicationData &sl_data,
                                        const Ale::DisconnectIndicationData &ale_data)
{
    vrunlog << "Subset_026 (onDisconnectIndication): получен сигнал Disconnect Indication от SL";
    uint32_t  brus_id = 0;
    auto params = _connection_table->getParametersByTcepid(ale_data.tcepidrcv);
    if (params != nullptr)
    {
        brus_id = params->brus_id;
    }

    if (_connection_table->removeConnection(ale_data.tcepidrcv))
    {
        vdeb << "Subset_026: Delete params with TCEPID" << ale_data.tcepidrcv << "successful!";
        _channel_state->stopSending();
        disconnect_indication(sl_data, ale_data, brus_id);
    }
    else
    {
        vfatal << "Subset_026: parameters with tcepid" << ale_data.tcepidrcv << "is not exists!";
    }
}
//=======================================================================================
void Subset_026::onDataIndication(const Sl::DataIndicationData& msg_data)
{
    vdeb << "Subset_026: получен сигнал Data Indication";
    parseAndSend(msg_data);
}
//=======================================================================================
bool Subset_026::connectRequest(const int32_t brus_id, const int32_t number, const uint32_t etcsid)
{
    if ( _connection_table->getParametersByPeerEtcsid(etcsid) != nullptr || _connection_table->getParametersByBrus(brus_id) != nullptr )
    {
        vwarning << "Can't send ConnectRequest. Connection with ETCSID" << etcsid << "already exists.";
        return false;
    }

    auto conn_params = ConnectionTable::ConnParams(number,
                                                   brus_id,
                                                   etcsid,
                                                   ConnectionTable::State::WAIT_FOR_CON_RESP );

    _connection_table->insertConnection(etcsid, conn_params);

    vdeb << "Subset_026: sending ConnectRequest to" << etcsid;
    _sl->connectRequest(etcsid);

    return true;
}
//=======================================================================================
void Subset_026::disconnectRequest(const uint16_t tcepidxmt,
                                   const uint8_t  reason,
                                   const uint8_t  sub_reason)
{
    if (tcepidxmt < ConnectionTable::NO_TCEPID) //TODO check
    {
        vrunlog << "Subset_026 (disconnectRequest): Отправка Disconnect Request...";

        auto params = _connection_table->getParametersByTcepid(tcepidxmt);
        if (params != nullptr)
        {
            vrunlog << "Susbet-026 (disconnectRequest, params != nullptr): Erasing params with TCEPID"
                    << tcepidxmt << "and sending DisconnectRequest...";
            _sl->disconnectRequest(tcepidxmt, reason, sub_reason);
        }
    }
    else
    {
        vwarning << "Susbet_026: Attempt to disconnect invalid TCEPID =" << tcepidxmt;
    }
}
//=======================================================================================
void Subset_026::dataRequest(uint32_t brus_id, const std::string &message)
{
    //TODO сделать метод bool paramsExists
    auto params = _connection_table->getParametersByBrus(brus_id);
    if (params != nullptr)
        _sl->dataRequest(params->tcepid, message);
    else
        vfatal << "Susbet_026: Subset parameters with BRUS ID" << brus_id << "are no exists!";
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::ValidatedTrainDataMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (ValidatedTrainDataMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formValidatedTrainDataMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::MaRequestMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (MaRequestMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formMaRequestMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::TrainPositionReportMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (TrainPositionReportMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formTrainPositionReportMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::AckMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (AckMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formAckMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::AckOfESMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (AckOfESMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formAckOfESMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::EndOfMissionMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (EndOfMissionMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formEndOfMissionMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::InitiationCSMsg &msg)
{
    auto params = _connection_table->getParametersByBrus(msg.header.brus_id);
    if (params == nullptr)
    {
        if ( !connectRequest(msg.header.brus_id, msg.number.number, _rbc_id) ) return;

        // Проверяем, что параметры заполнились после коннекта
        params = _connection_table->getParametersByBrus(msg.header.brus_id);
        if (params != nullptr)
        {
            // Вставляем CSInitMsg для данного ETCSID в ожидающюю соединения мапу
            auto upd_msg = msg;
            upd_msg.header.time = getTimestamp();
            _connection_table->insertPendingCsInitMsg(params->peer_etcsid, upd_msg);
        }
    }
    else
    {
        vwarning << "Subset_026: Can't place CSInit to pending and establish connect with ETCSID" << params->peer_etcsid
                 << ". Connection already exists.";
        return;
    }
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::TerminationCSMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (TerminationCSMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formTerminationCSMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::StartOfMissionPrMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (StartOfMissionPrMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formStartOfMissionPrMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::connectResponse(const uint16_t tcepid,
                                 const uint32_t time,
                                 const bool     ack,
                                 const int8_t   last_balise_id,
                                 const int8_t   version)
{
    auto header = subset_026::Header(static_cast<uint8_t> (MessageID::SYS_VERSION),
                                     static_cast<uint16_t>(MessageLength::SYS_VERSION),
                                     time);
    auto rbc_header = subset_026::RbcHeader(header,
                                            ack,
                                            last_balise_id);
    subset_026::SystemVersionMsg msg(rbc_header, version);
    sendMessage(tcepid, msg);
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::AckOfTerminationCSMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (AckOfTerminationCSMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formAckOfTerminationCSMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::NoCVSMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (NoCVSMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formNoCVSMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::SessionEstablishedMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (SessionEstablishedMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formSessionEstablishedMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(const subset_026::ActualDistanceMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (ActualDistanceMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(upd_msg.header.brus_id, to_bin::formActualDistanceMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendChannelStateMsg(bool is_normal_alive, bool is_redundant_alive)
{
    vdeb << "Subset_026: Отправка Data Request (ChannelStateMsg)...";

    auto params = _connection_table->getParametersByPeerEtcsid(_rbc_id);
    if (params != nullptr)
    {
        subset_026::Header header(static_cast<uint8_t> (MessageID::CHANNEL_STATE),
                                  static_cast<uint16_t>(MessageLength::CHANNEL_STATE),
                                  getTimestamp());
        subset_026::BrusHeader brus_header(header, params->brus_id);
        subset_026::ChannelStateMsg msg(brus_header,
                                        is_normal_alive,
                                        is_redundant_alive);
        dataRequest(params->brus_id, to_bin::formChannelStateMsg(msg).DataInString());
    }
    else
        vfatal << "Subset_026: Subset parameters with ETCSID" << _rbc_id << "are no exists!";
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::SrAuthMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (SrAuthMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formSrAuthMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::MovementAuthorityMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (MovementAuthorityMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formMovementAuthorityMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::AckOfTrainDataMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (AckOfTrainDataMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formAckOfTrainDataMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::UnconditionalESMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (UnconditionalESMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formUnconditionalESMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::RevocationOfESMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (RevocationOfESMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formRevocationOfESMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::GeneralMessage &msg)
{
    vdeb << "Subset_026: Отправка Data Request (GeneralMessage)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formGeneralMessage(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::SystemVersionMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (SystemVersionMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formSystemVersionMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::TrainRejectedMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (TrainRejectedMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formTrainRejectedMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::sendMessage(uint32_t brus_id, const subset_026::TrainAcceptedMsg &msg)
{
    vdeb << "Subset_026: Отправка Data Request (TrainAcceptedMsg)...";
    auto upd_msg = msg;
    upd_msg.header.time = getTimestamp();
    dataRequest(brus_id, to_bin::formTrainAcceptedMsg(upd_msg).DataInString());
}
//=======================================================================================
void Subset_026::parseAndSend(const Sl::DataIndicationData &msg_data)
{
    size_t bit_iterator = 0;
    auto data           = msg_data.user_data.udata();
    bool channel        = msg_data.channel;

    if (!_connection_table->updateConnectionState(msg_data.tcepid))
    {
        vwarning << "Subset_026: Parameters with TCEPID" << msg_data.tcepid << "is not contain in Subset Parameters!";
        return;
    }

    auto header = to_dto::parseHeader   (data, bit_iterator);
    auto id     = static_cast<MessageID>(header.id);

    vdeb << "Subset_026: Receive" << id;

    //TODO надо ли будет оставлять это в релизе?
    if (isRbcMessage(id) && _lag_control_on)
        _lag_controller->addTimestamp(getRbcId(), channel, header.time, getTimestamp());

    if(!checkTimestamp(getRbcId(), header.time))
    {
        vfatal << "Subset-026: Timestamp is invalid! Message rejected";
        return;
    }

    switch (id)
    {
        case MessageID::SR_AUTHORISATION:
        {
            _interface.dtoMsg(to_dto::parseSrAuthMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::MOVEMENT_AUTHORITY:
        {
            _interface.dtoMsg(to_dto::parseMovementAuthorityMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ACK_TRAIN_DATA:
        {
            _interface.dtoMsg(to_dto::parseAckOfTrainDataMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::UNCONDITIONAL_ES:
        {
            _interface.dtoMsg(to_dto::parseUnconditionalESMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::REVOCATION_OF_ES:
        {
            _interface.dtoMsg(to_dto::parseRevocationOfESMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::GENERAL_MSG:
        {
            _interface.dtoMsg(to_dto::parseGeneralMessage(data, bit_iterator, header));
            break;
        }
        case MessageID::SYS_VERSION:
        {
            _interface.dtoMsg(to_dto::parseSystemVersionMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ACK_TERMINATION:
        {
            vrunlog << "Subset-026 (parseAndSend): disconnect on receive of AckOfTerminationCSMsg";
            _interface.dtoMsg(to_dto::parseAckOfTerminationCSMsg(data, bit_iterator, header));
            disconnectRequest(msg_data.tcepid, 0, 0);
            break;
        }
        case MessageID::REJECTED:
        {
            _interface.dtoMsg(to_dto::parseTrainRejectedMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ACCEPTED:
        {
            _interface.dtoMsg(to_dto::parseTrainAcceptedMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::VALIDATED_TRAIN_DATA:
        {
            _interface.dtoMsg(to_dto::parseValidatedTrainDataMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::MA_REQUEST:
        {
            _interface.dtoMsg(to_dto::parseMaRequestMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::TRAIN_POSITION_REPORT:
        {
            _interface.dtoMsg(to_dto::parseTrainPositionReportMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ACKNOWLEDGEMENT:
        {
            _interface.dtoMsg(to_dto::parseAckMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ES_ACKNOWLEDGEMENT:
        {
            _interface.dtoMsg(to_dto::parseAckOfESMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::END_OF_MISSION:
        {
            _interface.dtoMsg(to_dto::parseEndOfMissionMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::NO_COMPATIBLE_VERSION:
        {
            _interface.dtoMsg(to_dto::parseNoCVSMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::CS_ESTABLISHED:
        {
            _interface.dtoMsg(to_dto::parseSessionEstablishedMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::CS_INITIATION:
        {
            auto msg = to_dto::parseInitiationCSMsg(data, bit_iterator, header);
            auto init_params = _connection_table->getParametersByTcepid(msg_data.tcepid);
            if (init_params != nullptr)
            {
                init_params->brus_id = msg.header.brus_id;
                init_params->number  = msg.number.number;
                _interface.dtoMsg(msg);
            }
            else
            {
                vwarning << "Subset-026: Can't handle CSInit.";
            }

            break;
        }
        case MessageID::CS_TERMINATION:
        {
            _interface.dtoMsg(to_dto::parseTerminationCSMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::START_OF_MISSION_PR:
        {
            _interface.dtoMsg(to_dto::parseStartOfMissionPrMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::CHANNEL_STATE:
        {
            _interface.dtoMsg(to_dto::parseChannelStateMsg(data, bit_iterator, header));
            break;
        }
        case MessageID::ACTUAL_DISTANCE:
        {
            _interface.dtoMsg(to_dto::parseActualDistanceMsg(data, bit_iterator, header));
            break;
        }
        default:
        {
            vwarning << "Subset-026: Unexpected ID!";
            break;
        }
    }
}
//=======================================================================================
uint32_t Subset_026::getTimestamp() const
{
    uint32_t now   = VTimePoint::now().milliseconds().count();
    uint32_t start = _start_time.milliseconds().count();
    return (now - start) / 10;
}
//=======================================================================================
void Subset_026::onALEConnectIndication(const uint16_t tcepid, const uint32_t etcsid)
{
    //TODO избавиться от этого перегруза
    if ( _connection_table->getParametersByBrus(tcepid) == nullptr && _connection_table->getParametersByPeerEtcsid(etcsid) == nullptr )
        _connection_table->insertConnection(etcsid, ConnectionTable::ConnParams(tcepid, etcsid));
    else
        vwarning << "Subset-026: Can't insert parameters with ETCSID"
                 << etcsid << "and TCEPID" << tcepid << "- already exists";
}
//=======================================================================================
void Subset_026::onALEConnectConfirmation(const uint16_t tcepid, const uint32_t etcsid)
{
    vdeb << "Subset-026: Received Connect Confirmation from ALE.";

    // TODO убрать в метод
    auto params = _connection_table->getParametersByPeerEtcsid(etcsid);
    if (params != nullptr)
    {
        params->tcepid = tcepid;
    }
    else
    {
        vrunlog << "Subset-026 (onALEConnectConfirmation): Can't find connection with ETCSID" << etcsid << "Disconnecting...";
        disconnectRequest(tcepid, 0, 0);
    }
}
//=======================================================================================
uint32_t Subset_026::getRbcId() const
{
    return _rbc_id;
}
//=======================================================================================
void Subset_026::setNoCsInit(bool state)
{
    _connection_table->setNoCsInit(state);
}
//======================================================================================
bool Subset_026::checkTimestamp(int32_t rbc_id, uint32_t new_t_train)
{
    auto params = _connection_table->getParametersByPeerEtcsid(rbc_id);
    if (params != nullptr)
    {
        if (params->last_t_train == 0)
        {
            params->last_t_train = new_t_train;
            return true;
        }

        if (params->last_t_train <= new_t_train)
        {
            auto delta = new_t_train - params->last_t_train;

            if (delta < _allowed_diff)
            {
                params->last_t_train = new_t_train;
                return true;
            }
            else
            {
                vwarning << "Subset-026: difference between message timestamps is more than allowed! Message rejected."
                         << "Allowed difference:" << _allowed_diff << "Calculated difference:" << delta;
                return false;
            }
        }
        else
        {
            vwarning << "Subset-026: timestamp of the new received message is less than previous one!"
                     << "Received timestamp:" << new_t_train << "Previous timestamp:" << params->last_t_train;
            return false;
        }
    }

    return false;
}
//=======================================================================================
std::map<uint32_t, ConnectionTable::ConnParams> Subset_026::getParameters()
{
    return _connection_table->getParametersMap();
}
//=======================================================================================
void Subset_026::breakSession()
{
    const auto params = _connection_table->getParametersByPeerEtcsid(_rbc_id);
    if ( params != nullptr)
        disconnectRequest(params->tcepid, 0, 0);
    else
        vwarning << "Subset-026: can't break session, params with ETCSID" << _rbc_id << "is not exists";
}
//=======================================================================================
void Subset_026::disconnectByPeer(uint32_t peer_etcsid, uint8_t reason, uint8_t sub_reason)
{
    _sl->disconnectByPeer(peer_etcsid, reason, sub_reason);
}

}
}