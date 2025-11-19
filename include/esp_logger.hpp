#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");
namespace ESPLogger {
    int get_count();
    void clear_logs();
    void log(std::string msg);
    void export_logs(void);
};
