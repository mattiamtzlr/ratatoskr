#include "util.hpp"
#include <Adafruit_VL53L1X.h>
#include <Wire.h>
// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms! (from lib)
#define TIMING_BUDGET 50

enum SensorPosition {
    LEFT = 0,         // side-left
    FRONT_LEFT = 1,   // top-left
    FRONT_RIGHT = 2,  // top-right
    RIGHT = 3         // side-right
};

class ToF {
   public:
    // Constructor performs initialization
    ToF(SensorPosition position, uint8_t i2c_address);

    // Start continuous ranging
    void start(TwoWire *wire = &Wire);

    // Read current distance in millimeters
    uint16_t read();

    bool dataReady();
    // The sensor this is
    const SensorPosition position;
    const uint8_t i2c_address;

   private:
    Adafruit_VL53L1X m_sensor;
};
