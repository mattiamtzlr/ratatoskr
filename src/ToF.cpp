#include "tof.hpp"

ToF::ToF(SensorPosition position, uint8_t i2c_address)
    : position(position), i2c_address(i2c_address) {
    m_sensor = Adafruit_VL53L1X(0, 0);
}

void ToF::start(TwoWire *wire) {
    if (!m_sensor.begin(0x29, &Wire)) {
        while (1) delay(10);
    }
    if (!m_sensor.startRanging()) {
        while (1) delay(10);
    }
    // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
    m_sensor.setTimingBudget(50);
}

bool ToF::dataReady() { return m_sensor.dataReady();}

uint16_t ToF::read() {
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
