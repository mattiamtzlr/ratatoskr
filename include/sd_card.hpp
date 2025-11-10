#pragma once
#include "util.hpp"
#include <SD.h>

#include <cstddef>
#include <cstdint>

enum LogLevel { LVL_LOW, LVL_NORMAL, LVL_CRITICAL };

class SDCard {
   private:
    const uint8_t m_chip_select = 5;
    File m_file;

   public:
    SDCard(const uint8_t chip_select);
    int init_log(void);
    void log_msg(LogLevel lvl, String msg);
    void end_log(void);
};
