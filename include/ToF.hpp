#pragma once
#include "logger.hpp"
#include "util.hpp"
#include <VL53L1X.h> 
#include <Wire.h>
#pragma once
// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms! (from lib)
constexpr uint32_t TIMING_BUDGET   = 15;  // in ms

constexpr uint8_t ADDR_LEFT        = 0x30;
constexpr uint8_t ADDR_FRONT_LEFT  = 0x31;
constexpr uint8_t ADDR_FRONT_RIGHT = 0x32;
constexpr uint8_t ADDR_RIGHT       = 0x33;  // 7-bit addresses

enum SensorPosition {
    LEFT        = 0,        // side-left
    FRONT_LEFT  = 1,        // front-left
    FRONT_RIGHT = 2,        // front-right
    RIGHT       = 3         // side-right
};

class ToF : public Logger {
   public:
    // Constructor performs initialization
    ToF(SensorPosition position, uint8_t i2c_address, uint8_t xshut_pin);

    // Start continuous ranging
    void start();

    // Read current distance in millimeters
    uint16_t read();

    bool dataReady();
    // The sensor this is
    const SensorPosition position;
    const uint8_t i2c_address;
    const int8_t xshut_pin;

   private:
    VL53L1X m_sensor;
};
