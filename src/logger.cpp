#include "logger.hpp"

#include <Arduino.h>

Logger::Logger(int start_index) { m_head_index = start_index; }

void Logger::log(std::string msg) {
    DatabaseError_t result =
        nvsDB.putPair(std::to_string(m_head_index++).c_str(), msg.c_str());
}
void Logger::export_logs(void) {
    int index = 0;
    while (index < m_head_index) {
        char value[100];
        size_t maxSize = sizeof(value);
        DatabaseError_t result = nvsDB.getValueOf(
            std::to_string(m_head_index).c_str(), value, &maxSize);
        Serial.print(m_head_index);
        Serial.print(": ");
        Serial.print(value);
        Serial.print('\n');
    }
}
