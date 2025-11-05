// #include "ToF.hpp"

// ToF::ToF(SensorPosition position, uint8_t shutdown_pin,
//                        uint8_t irq_pin, uint8_t i2c_address, TwoWire *wire = &Wire)
//     : position(position) {
//     m_sensor = Adafruit_VL53L1X(shutdown_pin, irq_pin);
//     if (m_sensor.begin(i2c_address, wire)) {
//         m_sensor.setTimingBudget(TIMING_BUDGET);  // 50 ms per measurement
//     } else {
//         throw -1;
//     }
// }

// void ToF::start() { m_sensor.startRanging(); }

// uint16_t ToF::read() {
//     if (m_sensor.dataReady()) {
//         uint16_t distance = m_sensor.distance();
//         m_sensor.clearInterrupt();
//         return distance;
//     }
//     return 0;  // return 0 if not ready (same output in library)
//     /*
//         Good check we could keep (from the library example file)
//         if (distance == -1) {
//             // something went wrong!
//             Serial.print(F("Couldn't get distance: "));
//             Serial.println(vl53.vl_status);
//             return;
//         }
//     */
// }
