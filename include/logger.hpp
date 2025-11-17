#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");
class Logger {
   private:
    void increment_count();
    int get_count();

   public:
    void clear();
    void log(std::string msg);
    void export_logs(void);
};
