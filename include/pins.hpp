#include <cstdint>
// ToF
constexpr uint8_t TOF_LEFT_XSHUT        = 0x15;
constexpr uint8_t TOF_FRONT_LEFT_XSHUT  = 0x18;
constexpr uint8_t TOF_FRONT_RIGHT_XSHUT = 0x19;
constexpr uint8_t TOF_RIGHT_XSHUT       = 0x23;

// I2C
constexpr uint8_t SDA = 0x21;
constexpr uint8_t SCL = 0x22;

// Motor / Encoder

/* left motor, pins in order of ESP => motor A resp. 1 */
constexpr uint8_t MOTOR_L_IN1       = 0x12;
constexpr uint8_t MOTOR_L_IN2       = 0x14;
constexpr uint8_t ENC_L_OUT1        = 0x33;
constexpr uint8_t ENC_L_OUT2        = 0x32;

/* right motor, pins in order of ESP => motor B resp. 2 */
constexpr uint8_t MOTOR_R_IN1       = 0x27;
constexpr uint8_t MOTOR_R_IN2       = 0x26;
constexpr uint8_t ENC_R_OUT1        = 0x35;
constexpr uint8_t ENC_R_OUT2        = 0x34;

