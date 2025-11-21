#include "ToF.hpp"

#include <Arduino.h>

ToF::ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin)
    : position(position),
      i2c_address(i2c_address),
      xshut_pin(xshut_pin),
      m_offset_mm(0) {}

void ToF::begin() {
    digitalWrite(xshut_pin, LOW);
    delay(50);
    digitalWrite(xshut_pin, HIGH);
    delay(5);

    if (!m_sensor.init()) {
        log("VL53L1X at pin" + std::to_string(xshut_pin) +
            "failed to initialize");
        Serial.printf("VL53L1X at pin %d failed to initialize\n", xshut_pin);
        return;
    }

    switch (position) {
        case LEFT:
            m_offset_mm = TOF_LEFT_OFFSET_MM;
            break;
        case FRONT_LEFT:
            m_offset_mm = TOF_FRONT_LEFT_OFFSET_MM;
            break;
        case FRONT_RIGHT:
            m_offset_mm = TOF_FRONT_RIGHT_OFFSET_MM;
            break;
        case RIGHT:
            m_offset_mm = TOF_RIGHT_OFFSET_MM;
            break;
    }

    // Assign the unique I2C address for this sensor and keep it enabled
    m_sensor.setAddress(i2c_address);
    delay(100);

    log("VL53L1X at pin" + std::to_string(xshut_pin) + "initialized");

    m_sensor.startContinuous(TIMING_BUDGET);

    // Discard the initial few readings
    for (int i = 0; i < 10; i++) {
        read();
    }
}

uint16_t ToF::read() {
    uint16_t distance = m_sensor.read();
    distance += m_offset_mm;
    return distance;
}
uint16_t ToF::get_reading() {
    if (m_sensor.dataReady()) m_reading = read();
    return m_reading;
}

void ToF::calibrate_sensor(uint16_t expected_distance) {
    uint32_t start = millis();
    uint32_t sum = 0;
    uint32_t count = 0;

    while (millis() - start < TOF_CALIBRATION_DURATION_MS) {
        while (!m_sensor.dataReady());
        uint16_t distance = m_sensor.read();
        sum += distance;
        /* for some reason the first couple measurements are zero, so don't
         * count those */
        count += (distance > 0);
        delay(TOF_CALIBRATION_SAMPLE_INTERVAL_MS);
    }

    if (count == 0) return;

    uint16_t average = sum / count;
    Serial.printf(
        "ToF Sensor at pin %d calibration: expected %d mm, average reading %d "
        "mm\n",
        xshut_pin, expected_distance, average);
    int16_t offset =
        static_cast<int16_t>(expected_distance) - static_cast<int16_t>(average);
    Serial.printf(" => Calculated offset: %d mm\n", offset);
    m_offset_mm = offset;
}
