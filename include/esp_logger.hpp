#pragma once
#include <string>

#include "NVSDatabase.hpp"
#include "util.hpp"

static NVSDatabase nvsDB("logs");

extern bool logger_enabled;

namespace ESPLogger {
int get_count();
void clear_logs();
void log(std::string msg);
void force_log(std::string msg);
void export_logs(void);
std::string get_tail();
void write_solution(const std::vector<Instruction>& instr);
bool retrieve_solution(std::vector<Instruction>& instr);
};
