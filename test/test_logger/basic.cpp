

#include <Arduino.h>

#include "logger.hpp"

Logger myLogs;

void populate() {
    myLogs.begin(0);
    int i = 0;
    while (i < 10) {
        std::string msg = std::to_string(i++);
        myLogs.log(msg);
        delay(100);
    }
}
void read() {
    myLogs.begin(11);
    myLogs.export_logs();
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    // populate();
    read();
    // myLogs.clear();
    Serial.println("DONE");
}

void loop() {}
