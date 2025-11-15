#include <Arduino.h>
#include "ToF.hpp"

ToF::ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin)
    : position(position), i2c_address(i2c_address), xshut_pin(xshut_pin) {

        digitalWrite(xshut_pin, HIGH);
        delay(1000); // give time to boot

        if (!m_sensor.init()) {
            Serial.printf("VL53L1X at pin %d failed to init\n", xshut_pin);
            return ;
        }
        m_sensor.setAddress(i2c_address);
        delay(100);
        digitalWrite(xshut_pin, LOW);
        delay(100);
        
        Serial.printf("VL53L1X at pin %d initialized\n", xshut_pin);
}
// Is this needed?
void ToF::start() {
    m_sensor.startContinuous(TIMING_BUDGET);
}

bool ToF::dataReady() { 
    return m_sensor.dataReady();
}

uint16_t ToF::read() {
    if (m_sensor.dataReady()) {
        uint16_t distance = m_sensor.read();
        return distance;
    }
    return -1;
}
