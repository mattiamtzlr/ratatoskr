#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");
class Logger {
   private:
    int m_head_index = 0;
    void increment_count();
    int get_count();

   public:
    void clear();
    void begin();
    void log(std::string msg);
    void export_logs(void);
};
