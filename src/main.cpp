

#include <Arduino.h>

#include "logger.hpp"

Logger myLogs;

void populate() {
    for (int i = 0; i < 10; i++) {
        std::string msg = std::to_string(i++);
        myLogs.log(msg);
        delay(100);
    }
}
void read() { myLogs.export_logs(); }

void setup() {
    Serial.begin(115200);
    delay(2000);
    myLogs.begin();
    // populate();
    read();
    // myLogs.clear();
    Serial.println("DONE");
}

void loop() {}
