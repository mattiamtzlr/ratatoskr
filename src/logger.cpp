#include "logger.hpp"

#include <Arduino.h>

void Logger::begin() { m_head_index = get_count(); }

void Logger::increment_count() {
    int next_count = get_count() + 1;
    nvsDB.putPair("0", std::to_string(next_count).c_str());
    m_head_index = next_count;
}

int Logger::get_count() {
    char value[10];
    size_t maxSize = sizeof(value);
    nvsDB.getValueOf("0", value, &maxSize);
    return atoi(value);
}

void Logger::log(std::string msg) {
    nvsDB.putPair(std::to_string(m_head_index).c_str(), msg.c_str());
    increment_count();
}

void Logger::clear() {
    nvsDB.eraseAll();
    nvsDB.putPair("0", "0");
}

void Logger::export_logs(void) {
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
