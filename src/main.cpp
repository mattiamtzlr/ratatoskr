#include <Arduino.h>

#include "logger.hpp"

Logger myLogs(0);
void setup() {
    Serial.begin(115200);
    myLogs.log("Setup done");
}

void loop() {
    int i = 0;
    while (i++ < 10) {
        myLogs.log("get logged");
        delay(2000);
    }
    myLogs.export_logs();
}
