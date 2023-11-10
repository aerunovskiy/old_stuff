#ifndef LAG_CONTROLLER_H
#define LAG_CONTROLLER_H

#include <sstream>
#include <fstream>
#include <map>

#include "vlog.h"
#include "vtimepoint.h"

namespace euroradio
{

namespace subset_026
{

class LagController
{
public:
    explicit LagController(const std::string &log_folder_path);

    void addTimestamp(uint32_t etcsid,
                      bool is_normal,
                      uint32_t received_timestamp,
                      uint32_t local_timestamp);

private:
    struct MessageTimestamps
    {
        uint32_t last_received = 0;
        uint32_t last_local = 0;
    };

    struct Channels
    {
        MessageTimestamps normal;
        MessageTimestamps redundant;
    };

private:
    std::string getChannelName(bool channel_number) const;

    void log(const std::string &str);
    void logLag(uint32_t etcsid,
                bool is_normal,
                uint32_t received_diff,
                uint32_t local_diff);
    void startLog();

private:
    std::map<uint32_t, Channels> _timestamps_table;
    std::string _log_path;
};

}

}

#endif //LAG_CONTROLLER_H