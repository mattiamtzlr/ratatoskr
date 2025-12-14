#include "esp_logger.hpp"

#include <Arduino.h>

#include <string>

#include "NVSDatabase.hpp"
#include "util.hpp"

bool logger_enabled = false;

int ESPLogger::get_count() {
    char value[10];
    size_t maxSize = sizeof(value);
    nvsDB.getValueOf("0", value, &maxSize);
    return atoi(value);
}

void ESPLogger::log(std::string msg) {
    if (logger_enabled) {
        force_log(msg);
    }
}

void ESPLogger::force_log(std::string msg) {
    nvsDB.putPair(std::to_string(get_count()).c_str(), msg.c_str());
    int next_count = get_count() + 1;
    nvsDB.putPair("0", std::to_string(next_count).c_str());
}

void ESPLogger::clear_logs() {
    nvsDB.eraseAll();
    nvsDB.putPair("0", "2");  // Stores total size of log
    nvsDB.putPair("1", "0");  // Stores the index of the most recent solution
}

void ESPLogger::export_logs(void) {
    int index = 0;
    int num_entries = get_count();

    Serial.print(num_entries);
    Serial.println(" entries in the DB");

    while (index < num_entries) {
        char value[80];
        size_t maxSize = sizeof(value);
        DatabaseError_t result =
            nvsDB.getValueOf(std::to_string(index).c_str(), value, &maxSize);
        Serial.print(index++);
        Serial.print(": ");
        Serial.print(value);
        Serial.print('\n');
    }
}

std::string ESPLogger::get_at(int i) {
    char value[80];
    size_t maxSize = sizeof(value);
    DatabaseError_t err =
        nvsDB.getValueOf(std::to_string(i).c_str(), value, &maxSize);
    return std::string(value);
}

std::string ESPLogger::get_tail() { return get_at(get_count() - 1); }

std::string ESPLogger::get_prev_sol() {
    return get_at(atoi(get_at(1).c_str()));
}

void ESPLogger::clear_sol() { nvsDB.putPair("1", "0"); }

void ESPLogger::write_solution(const std::vector<Instruction>& instr) {
    std::string to_write = "SOL";
    for (const Instruction& i : instr) {
        switch (i) {
            case (MOVE_FORWARD):
                to_write += 'F';
                break;
            case (MOVE_FORWARD_HALF):
                to_write += 'f';
                break;
            case (TURN_LEFT_45):
                to_write += 'l';
                break;
            case (TURN_LEFT_90):
                to_write += 'L';
                break;
            case (TURN_RIGHT_45):
                to_write += 'r';
                break;
            case (TURN_RIGHT_90):
                to_write += 'R';
                break;
            case (BLANK):
                to_write += '_';
                break;
        }
    }
    nvsDB.putPair("1", std::to_string(get_count()).c_str());
    force_log(to_write);
}

bool ESPLogger::retrieve_solution(std::vector<Instruction>& instr) {
    std::string tail = get_prev_sol();
    if (tail.substr(0, 3) == "SOL") {
        for (const char& i : tail.substr(3)) {
            switch (i) {
                case ('F'):
                    instr.push_back(MOVE_FORWARD);
                    break;
                case ('f'):
                    instr.push_back(MOVE_FORWARD_HALF);
                    break;
                case ('l'):
                    instr.push_back(TURN_LEFT_45);
                    break;
                case ('L'):
                    instr.push_back(TURN_LEFT_90);
                    break;
                case ('r'):
                    instr.push_back(TURN_RIGHT_45);
                    break;
                case ('R'):
                    instr.push_back(TURN_RIGHT_90);
                    break;
                case ('_'):
                    instr.push_back(BLANK);
                    break;
                default:
                    return false;
            }
        }
        return true;
    }
    return false;
}
