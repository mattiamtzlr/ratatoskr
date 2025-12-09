#pragma once
#include <VL53L1X.h>
#include <Wire.h>

#include "config.hpp"
#include "esp_logger.hpp"
#include "util.hpp"

// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms! (from lib)
constexpr uint32_t TIMING_BUDGET = 20;  // in ms
constexpr uint32_t TOF_CALIBRATION_DURATION_MS = 3000;
constexpr uint32_t TOF_CALIBRATION_SAMPLE_INTERVAL_MS = 10;

enum SensorPosition {
    LEFT = 0,         // side-left
    FRONT_LEFT = 1,   // front-left
    FRONT_RIGHT = 2,  // front-right
    RIGHT = 3         // side-right
};

class ToF {
   public:
    // Constructor performs initialization
    ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin);

    // Start continuous ranging
    void begin();

    // Read current corrected distance in millimeters
    uint16_t read();
    uint16_t get_reading();

    void calibrate_sensor(uint16_t expected_distance);

    // Physical position on the mouse
    const SensorPosition position;

    const uint8_t i2c_address;
    const int8_t xshut_pin;

   private:
    uint16_t m_reading;
    VL53L1X m_sensor;
    int16_t m_offset_mm = 0;
};
