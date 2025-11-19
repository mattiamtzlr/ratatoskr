#pragma once
#include "loggable.hpp"
#include "util.hpp"
#include <VL53L1X.h> 
#include <Wire.h>

// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms! (from lib)
constexpr uint32_t TIMING_BUDGET   = 15;  // in ms
constexpr uint32_t TOF_CALIBRATION_DURATION_MS = 3000;
constexpr uint32_t TOF_CALIBRATION_SAMPLE_INTERVAL_MS = 10;

constexpr uint16_t TOF_SIDE_EXPECTED_MM = 50;       // -----------------
constexpr uint16_t TOF_FRONT_EXPECTED_MM = 40;      // | measured and  |
constexpr int16_t  TOF_LEFT_OFFSET_MM = -33;        // | calibrated in |
constexpr int16_t  TOF_FRONT_LEFT_OFFSET_MM = -18;  // | calibration   |
constexpr int16_t  TOF_FRONT_RIGHT_OFFSET_MM = 2;   // | cage (TM)     |
constexpr int16_t  TOF_RIGHT_OFFSET_MM = -7;        // -----------------

enum SensorPosition {
    LEFT        = 0,        // side-left
    FRONT_LEFT  = 1,        // front-left
    FRONT_RIGHT = 2,        // front-right
    RIGHT       = 3         // side-right
};

class ToF : public Loggable {
   public:
    // Constructor performs initialization
    ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin);

    // Start continuous ranging
    void begin();

    // Read current corrected distance in millimeters
    uint16_t read();

    void calibrate_sensor(uint16_t expected_distance);

    // Physical position on the mouse 
    const SensorPosition position;

    const uint8_t i2c_address;
    const int8_t xshut_pin;

   private:
    VL53L1X m_sensor;
    int16_t m_offset_mm = 0;
};
