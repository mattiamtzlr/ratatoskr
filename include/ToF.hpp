#include <Adafruit_VL53L1X.h>
#include <Wire.h>

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms! (from lib)
#define TIMING_BUDGET 50

enum SensorPosition {
    LEFT        = 0,  // side-left
    FRONT_LEFT  = 1,  // top-left
    FRONT_RIGHT = 2,  // top-right
    RIGHT       = 3   // side-right
};

class ToFVL53L1X {
   public:
    // Constructor performs initialization
    ToFVL53L1X(SensorPosition position, TwoWire *wire = &Wire, uint8_t i2c_address = 0x29);

    // Start continuous ranging
    void start();

    // Read current distance in millimeters
    uint16_t read();

   private:
    SensorPosition m_position;
    Adafruit_VL53L1X m_sensor;
};
