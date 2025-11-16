#pragma once
#include <cstdint>
// ToF
constexpr uint8_t TOF_LEFT_XSHUT        = 15;
constexpr uint8_t TOF_FRONT_LEFT_XSHUT  = 18;
constexpr uint8_t TOF_FRONT_RIGHT_XSHUT = 19;
constexpr uint8_t TOF_RIGHT_XSHUT       = 23;

// I2C
#if (defined SDA && defined SCL)
constexpr uint8_t SDA = 0x21;
constexpr uint8_t SCL = 0x22;
#endif

// Motor / Encoder

/* left motor, pins in order of ESP => motor A resp. 1 */
constexpr uint8_t MOTOR_L_IN1       = 12;
constexpr uint8_t MOTOR_L_IN2       = 14;
constexpr uint8_t ENC_L_OUT1        = 33;
constexpr uint8_t ENC_L_OUT2        = 32;

/* right motor, pins in order of ESP => motor B resp. 2 */
constexpr uint8_t MOTOR_R_IN1       = 27;
constexpr uint8_t MOTOR_R_IN2       = 26;
constexpr uint8_t ENC_R_OUT1        = 35;
constexpr uint8_t ENC_R_OUT2        = 34;
