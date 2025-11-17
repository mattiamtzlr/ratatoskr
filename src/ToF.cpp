#include "ToF.hpp"

#include <Arduino.h>

ToF::ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin)
    : position(position), i2c_address(i2c_address), xshut_pin(xshut_pin) {
        digitalWrite(xshut_pin, LOW);
        delay(50);
        digitalWrite(xshut_pin, HIGH);
        delay(5);
        if (!m_sensor.init(i2c_address)) {
            log("VL53L1X at pin" + std::to_string(xshut_pin) + "failed to init");
            return;
        }


        // Bring this sensor out of reset (XSHUT HIGH) and give it time to boot
        digitalWrite(xshut_pin, HIGH);
        delay(1000);

        if (!m_sensor.init()) {
            log("VL53L1X at pin" + std::to_string(xshut_pin) + "failed to init");
            return;
        }

        // Assign the unique I2C address for this sensor and keep it enabled
        m_sensor.setAddress(i2c_address);
        delay(100);

        log("VL53L1X at pin" + std::to_string(xshut_pin) + "initialized");
}


// Is this needed?
void ToF::start() {
    // Wire.begin() is called in setup(); just start continuous ranging here.
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
