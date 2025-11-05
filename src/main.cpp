#include <Arduino.h>

#include "ToF.hpp"

ToF *sensor = NULL;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    *sensor = ToF(LEFT, 34, 35, 0x29,  Wire);
    delay(1000);
    sensor->start();
    Serial.println("Starting: ");
}

void loop() {
    uint16_t reading = sensor->read();
    Serial.println(reading);
    delay(500);
}
