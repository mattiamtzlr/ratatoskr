#include "ToF.hpp"

ToFVL53L1X::ToFVL53L1X(SensorPosition position, TwoWire *wire, uint8_t i2c_address)
    : m_position(position) {
    if (m_sensor.begin(i2c_address, wire)) {
        m_sensor.setTimingBudget(TIMING_BUDGET);  // 50 ms per measurement
    } else {
        throw -1;
    }
}

void ToFVL53L1X::start() {
    m_sensor.startRanging();
}

uint16_t ToFVL53L1X::read() {
    if (m_sensor.dataReady()) {
        uint16_t distance = m_sensor.distance();
        m_sensor.clearInterrupt();
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
