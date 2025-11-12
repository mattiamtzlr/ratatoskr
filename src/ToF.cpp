#include <Arduino.h>
#include "tof.hpp"

ToF::ToF(SensorPosition position, uint8_t i2c_address, int8_t xshut_pin)
    : position(position), i2c_address(i2c_address), xshut_pin(xshut_pin) {
        pinMode(xshut_pin, OUTPUT);
        digitalWrite(xshut_pin, LOW);
        delay(50);
        digitalWrite(xshut_pin, HIGH);
        delay(5);
        if (!m_sensor.init(i2c_address)) {
            Serial.printf("VL53L1X at pin %d failed to init\n", xshut_pin);
            return ;
        }
}

void ToF::start(TwoWire *wire) {
    Wire.begin();
    m_sensor.startContinuous(TIMING_BUDGET);
}

bool ToF::dataReady() { return m_sensor.dataReady();}

uint16_t ToF::read() {
    if (m_sensor.dataReady()) {
        uint16_t distance = m_sensor.read();
        return distance;
    }
    return 0;  // return 0 if not ready (same output in library)
    /*
        Good check we could keep (from the library example file)
        if (distance == -1) {
            // something went wrong!
            Serial.print(F("Couldn't get distance: "));
            Serial.println(vl53.vl_status);
            return;
        }
    */
}
