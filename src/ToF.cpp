#include "ToF.hpp"

ToFVL53L1X::ToFVL53L1X(String label, TwoWire *wire, uint8_t i2c_address)
    : m_label(label), m_initialized(false) {
    if (m_sensor.begin(i2c_address, wire)) {
        m_sensor.setTimingBudget(50);  // 50 ms per measurement
        m_initialized = true;
    } else {
        Serial.print("Failed to initialize VL53L1X: ");
        Serial.println(m_label);
    }
}

void ToFVL53L1X::start() {
    if (m_initialized) {
        m_sensor.startRanging();
    } else {
        Serial.print("Cannot start VL53L1X (not initialized): ");
        Serial.println(m_label);
    }
}

uint16_t ToFVL53L1X::read() {
    // I could still do the struct stuff but maybe its
    // just easier to do this?
    if (m_initialized && m_sensor.dataReady()) {
        uint16_t distance = m_sensor.distance();
        m_sensor.clearInterrupt();
        return distance;
    }
    return 0;  // return 0 if not ready or not initialized
}
