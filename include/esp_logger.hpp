#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");

extern bool logger_enabled;

namespace ESPLogger {
int get_count();
void clear_logs();
void log(std::string msg);
void force_log(std::string msg);
void export_logs(void);
};  // namespace ESPLogger
std::string get_tail();
