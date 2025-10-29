#include <Adafruit_VL53L1X.h>
#include <Arduino.h>
#include <Wire.h>

class ToFVL53L1X {
   public:
    // Constructor performs initialization
    ToFVL53L1X(String label, TwoWire *wire = &Wire, uint8_t i2c_address = 0x29);

    // Start continuous ranging
    void start();

    // Read current distance in millimeters
    uint16_t read();

   private:
    String m_label;
    Adafruit_VL53L1X m_sensor;
    bool m_initialized;
};
