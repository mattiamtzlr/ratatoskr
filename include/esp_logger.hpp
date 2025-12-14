#pragma once
#include <string>

#include "NVSDatabase.hpp"
#include "util.hpp"

static NVSDatabase nvsDB("logs");

extern bool logger_enabled;

namespace ESPLogger {

/**
 * @returns the number of entries stored in flash.
 */
int get_count();

/**
 * Clears all logs out of flash memory.
 */
void clear_logs();

/**
 * Write a c++ string to flash memory on the ESP32.
 * @params the message to put into the log.
 * If logging is disabled this function does nothing.
 */
void log(std::string msg);

/**
 * Write a c++ string to flash memory on the ESP32.
 * @params the message to put into the log.
 * This will use the NVSDatabase library to write the
 * message to flash regardles of whether logging is
 * enabled or not.
 */
void force_log(std::string msg);

/**
 * Will dump the logged entries to Serial out.
 * Requires a Serial connection.
 */
void export_logs(void);

/**
 * Get logged string at index i
 * @params index i
 * Does not handle errors that happen in the library.
 * @returns message from flash
 */
std::string get_at(int i);

/**
 * Get logged string at last index
 * @returns message from flash
 */
std::string get_tail();

/**
 * Get logged solution string
 * @returns solution string from flash
 */
std::string get_prev_sol();

/**
 * Remove logged solution string
 */
void clear_sol();

/**
 * Write solution string to log
 * @params a vector of the instructions for the final run.
 */
void write_solution(const std::vector<Instruction>& instr);

/**
 * Create instruction vector from solution string in flash.
 * @params a vector of the instructions to be populated.
 */
bool retrieve_solution(std::vector<Instruction>& instr);
};  // namespace ESPLogger
