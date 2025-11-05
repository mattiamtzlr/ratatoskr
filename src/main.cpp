#include "Adafruit_VL53L1X.h"
#include "ToF.hpp"

ToF tof = ToF(LEFT, 0x29);  // rip xshut and irq

void setup() {
    Serial.begin(115200);

    Serial.println(F("Adafruit VL53L1X sensor demo"));

    Wire.begin();

    tof.start(&Wire);
}

void loop() {
    if (tof.dataReady()) Serial.println(tof.read());
}
