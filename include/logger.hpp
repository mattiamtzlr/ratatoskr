#pragma once
#include <string>

#include "NVSDatabase.hpp"


static NVSDatabase nvsDB("logs");
class Logger {
   private:
    int m_head_index = 0;

   public:
    Logger(int start_index);
    void log(std::string msg);
    void export_logs(void);
};
