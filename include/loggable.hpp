#pragma once
#include <string>

#include "NVSDatabase.hpp"

static NVSDatabase nvsDB("logs");
class Loggable {
   private:
    static void increment_count();
    static int get_count();

   public:
    static void clear_logs();
    static void log(std::string msg);
    static void export_logs(void);
};
