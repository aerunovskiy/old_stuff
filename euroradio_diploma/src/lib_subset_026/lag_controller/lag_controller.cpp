//=======================================================================================
#include "lag_controller.h"
//=======================================================================================
LagController::LagController(const std::string &log_folder_path)
: _log_path(log_folder_path + "lag.log")
{
    startLog();
}
//=======================================================================================
void LagController::addTimestamp(uint32_t etcsid,
                                 bool     is_normal,
                                 uint32_t received_timestamp,
                                 uint32_t local_timestamp)
{
    if (_timestamps_table.find(etcsid) == _timestamps_table.end())
    {
        auto log_string = "LagController: Start control lags of ETCSID = " + std::to_string(etcsid);
                        + " on " + getChannelName(is_normal) + " channel.\n";
        log(log_string);
        _timestamps_table.insert(std::make_pair(etcsid, Channels()));
    }

    auto ch = (is_normal) ? &_timestamps_table[etcsid].normal : &_timestamps_table[etcsid].redundant;

    if (ch->last_received == 0 && ch->last_local == 0)
    {
        ch->last_received = received_timestamp;
        ch->last_local    = local_timestamp;

        logLag(etcsid, is_normal, 0, 0);

        return;
    }

    auto received_diff = (received_timestamp - ch->last_received) * 10;
    auto local_diff    = (local_timestamp    - ch->last_local   ) * 10;

    logLag(etcsid, is_normal, received_diff, local_diff);

    ch->last_received = received_timestamp;
    ch->last_local    = local_timestamp;
}
//=======================================================================================
std::string LagController::getChannelName(bool channel_number) const
{
    return ( (channel_number) ? "RORS" : "GSM-R" );
}
//=======================================================================================
void LagController::startLog()
{
    std::stringstream ss;

    ss << "------------- BEGIN LOGGING ------------- " << VTimePoint::now().str_datetime_zzz()
       << " -----------------------" << std::endl;
    ss << "LRT = Last Received Timestamp, LLT = Last Local Timestamp, RTD = Received Timestamp Difference, "
          "LTD = Local Timestamp Difference." << std:: endl;

    std::ofstream log_file(_log_path, std::ios_base::app);
    log_file << ss.rdbuf();
    log_file.close();
}
//=======================================================================================
void LagController::log(const std::string &str)
{
    std::stringstream ss;
    ss << VTimePoint::now().str_datetime_zzz() << "\t";
    ss << str;

    std::ofstream log_file(_log_path, std::ios_base::app);
    log_file << str;
    log_file.close();
}
//=======================================================================================
void LagController::logLag(uint32_t etcsid,
                           bool     is_normal,
                           uint32_t received_diff,
                           uint32_t local_diff)
{
    std::stringstream ss;
    ss << VTimePoint::now().str_datetime_zzz() << "\t";

    auto ch = (is_normal) ? &_timestamps_table[etcsid].normal : &_timestamps_table[etcsid].redundant;

    ss << "BRUS ID = " << etcsid << " " << getChannelName(is_normal) << " LRT = " << ch->last_received
       << ", LLT = " << ch->last_local << ", RTD = " << received_diff << " ms, LTD = "
       << local_diff << " ms." << std::endl;

    std::ofstream log_file(_log_path, std::ios_base::app);
    log_file << ss.rdbuf();
    log_file.close();
}
//=======================================================================================