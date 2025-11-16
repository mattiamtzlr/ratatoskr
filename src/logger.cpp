#include "logger.hpp"

#include <Arduino.h>

void Logger::begin(int start_index) { m_head_index = start_index; }

void Logger::log(std::string msg) {
    DatabaseError_t result =
        nvsDB.putPair(std::to_string(m_head_index).c_str(), msg.c_str());
    m_head_index++;
}

void Logger::clear() { nvsDB.eraseAll(); }

void Logger::export_logs(void) {
    int index = 0;
    Serial.print(m_head_index);
    Serial.println(" entries in the DB");
    while (index < m_head_index) {
        char value[100];
        size_t maxSize = sizeof(value);
        DatabaseError_t result =
            nvsDB.getValueOf(std::to_string(index).c_str(), value, &maxSize);
        Serial.print(index++);
        Serial.print(": ");
        Serial.print(value);
        Serial.print('\n');
    }
    m_head_index = 0;
}
