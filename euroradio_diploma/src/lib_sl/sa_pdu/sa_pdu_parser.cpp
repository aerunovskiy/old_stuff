#include "sa_pdu_parser.h"
#include "iostream"
#include "vbyte_buffer_view.h"
#include "vcat.h"

SaPduParser::SaPduParser(std::shared_ptr<BasicAle> t)
{
    _t = t;
    _t->connect_indication   .connect(this, &SaPduParser::onConnectIndication    );
    _t->connect_confirmation .connect(this, &SaPduParser::onConnectConfirmation  );
    _t->data_indication      .connect(this, &SaPduParser::onDataIndication       );
    _t->disconnect_indication.connect(this, &SaPduParser::onDisconnectIndication );
}

void SaPduParser::onConnectIndication   (Ale::ConnectIndicationData    dt)
{
    parseAu1(vbyte_buffer(dt.user_data), dt.tcepida, dt.calling_etcsid);
}

void SaPduParser::onConnectConfirmation (Ale::ConnectConfirmationData  dt)
{
    parseAu2(vbyte_buffer(dt.user_data), dt.tcepidb, dt.responding_etcsid);
}

void SaPduParser::onDataIndication      (Ale::DataIndicationData       dt)
{
    parseAu3_Ar_Dt(vbyte_buffer(dt.user_data), dt.tcepidrcv, dt.channel);
}

void SaPduParser::onDisconnectIndication(Ale::DisconnectIndicationData dt)
{
    vdeb << "SL: Получен сигнал Disconnect Indication от ALE. Начинаю парсинг пакета DI...";
    vdeb << "SL: user_data size=" << dt.user_data.size();
    parseDi(dt);
}

void SaPduParser::parseAu1(vbyte_buffer    user_data,
                            uint16_t        tcepid,
                            uint32_t        Calling_etcsid)
{
    auto view = user_data.view();

    auto u = view.u8();
    Header header (u); // ETY + MTI + DF

    auto s = view.u32_BE();
    SaSaF sasaf (s); // SA + SaF

    auto rb = view.u64_BE();
    received_au1(Au1SaPdu(header.field.ety,
                           header.field.mti,
                           header.field.df,
                           sasaf.field.sa,
                           sasaf.field.saf,
                           rb
                           ),
                 tcepid,
                 Calling_etcsid
                );
}

void SaPduParser::parseAu2( vbyte_buffer    user_data,
                            uint16_t        tcepid,
                            uint32_t        responding_etcsid   )
{
    auto view = user_data.view();

    auto u = view.u8();
    Header header (u); // ETY + MTI + DF

    auto s = view.u32_BE();
    SaSaF sasaf (s); // SA + SaF

    auto ra  = view.u64_BE();
    auto mac = view.u64_BE();

    received_au2(Au2SaPdu(header.field.ety,
                           header.field.mti,
                           header.field.df,
                           sasaf.field.sa,
                           sasaf.field.saf,
                           ra,
                           mac),
                 tcepid );
}

void SaPduParser::parseAu3_Ar_Dt(vbyte_buffer user_data, uint16_t tcepid, uint8_t channel)
{
    auto view = user_data.view();

    auto u = view.u8();
    Header header (u); // ETY + MTI + DF

    switch(header.field.mti)
    {
        case SA_AU3:
        {
            auto mac = view.u64_BE();
            received_au3(Au3SaPdu(header.field.ety,
                                   header.field.mti,
                                   header.field.df,
                                   mac ),
                         tcepid
                         );
            break;
        }
        case SA_AR:
        {
            auto mac = view.u64_BE();
            received_ar(ArSaPdu(header.field.ety,
                                 header.field.mti,
                                 header.field.df,
                                 mac ),
                        tcepid
                        );
            break;
        }
        case SA_DT:
        {
            size_t data_length = user_data.size() - mac_size - header_size;

            auto str = view.string(data_length);
            auto mac = view.u64_BE();

            received_dt(    DtSaPdu (  header.field.ety,
                                        header.field.mti,
                                        header.field.df,
                                        vbyte_buffer(str),
                                        mac ),
                            tcepid, channel);
            break;
        }
        default:
        {
            vfatal << "SL: Неверный параметр MTI";
            return;
        }
    }
}

void SaPduParser::parseDi(Ale::DisconnectIndicationData dt)
{
    vbyte_buffer data(dt.user_data);
    if (data.size() != 0)
    {
        auto view = data.view();

        auto u          = view.u8();
        Header  header (u);      // ETY + MTI + DF

//        vdeb << "SL: Распаршен header (" << static_cast<uint16_t>(u) << ")";
//        vdeb << "ETY=" << static_cast<uint16_t>(header.field.ety);
//        vdeb << "MTI=" << static_cast<uint16_t>(header.field.mti);
//        vdeb << "DF=" << static_cast<uint16_t>(header.field.df);

        auto reason     = view.u8();
        auto sub_reason = view.u8();

        if (header.field.mti == SA_DI)
            received_di(DiSaPdu(header.field.df,
                                 reason,
                                 sub_reason),
                        dt,
                        dt.tcepidrcv
                        );
        else
        {
            vfatal << "SL: заголовок" << static_cast<uint16_t>(header.field.mti) << "не соответсвует SaDI:" << vcat(SA_DI).hex().str();
            disconnect_indication(dt, dt.tcepidrcv);
        }
    }
    else
    {
        vtrace << "SaPdu: socket_disconnected recieved";
        disconnect_indication(dt, dt.tcepidrcv);
    }
}

