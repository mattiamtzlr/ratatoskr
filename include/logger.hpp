#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");
class Logger {
   private:
    int m_head_index = 0;

   public:
    void clear();
    void begin(int start_index);
    void log(std::string msg);
    void export_logs(void);
};
