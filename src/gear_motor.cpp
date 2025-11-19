#include "gear_motor.hpp"

#define GEARING 31.5
#define ENCODERMULT 6

void IRAM_ATTR GearMotor::isr_trampoline(void *obj) {
    ((GearMotor *)obj)->encoder_interrupt();
}
/*
 * GearMotor class constructor
 */

GearMotor::GearMotor(int in1, int in2, int ch1, int ch2, int encoder_pin_1, int encoder_pin_2,
                     int max_pwm)
    : IN1(in1),
      IN2(in2),
      CH1(ch1),
      CH2(ch2),
      ENCODER_PIN_1(encoder_pin_1),
      ENCODER_PIN_2(encoder_pin_2), m_encoder_t_diff(1), m_delta_time(0), m_desired_rpm(0), m_t_last_i(0) {

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    ledcSetup(ch1, MOTOR_FREQ, MOTOR_RES);
    ledcSetup(ch2, MOTOR_FREQ, MOTOR_RES);
    ledcAttachPin(in1, ch1);
    ledcAttachPin(in2, ch2);
}

/**
 * executed on `ENCODER_PIN_1` input rising edge.
 */
void IRAM_ATTR GearMotor::encoder_interrupt() {
    // This function is heavily inspired from
    // https://www.adafruit.com/product/4640
    int t_curr_i = micros();
    if (m_t_last_i < t_curr_i) {
        m_delta_time = t_curr_i - m_t_last_i;
    }
    m_t_last_i = t_curr_i;
    // Count encoder ticks
    m_encoder_count++;
}

/**
 * Get the rpm of the motor
 */
int GearMotor::get_rpm() {
    // did not wrap around
    float revolutions = m_delta_time;  // us
    revolutions = 1.0 / revolutions;   // rev per us
    revolutions *= 1000000;            // rev per sec
    revolutions *= 60;                 // rev per min
    revolutions /= GEARING;            // account for gear ratio
    revolutions /= ENCODERMULT;  // account for multiple ticks per rotation
    return revolutions;
}

/**
 * Spin motor clockwise at rpm
 */
void GearMotor::spin_cw(int rpm) {
    ledcWrite(CH1, 0);
    ledcWrite(CH2, rpm);
}

/**
 * Spin motor counter-clockwise at rpm
 */
void GearMotor::spin_ccw(int rpm) {
    ledcWrite(CH2, 0);
    ledcWrite(CH1, rpm);
}

/**
 * Stop motor stopping acceleration
 */
void GearMotor::coast() {
    ledcWrite(CH1, 255);
    ledcWrite(CH2, 255);
}

/**
 * Stop motor by forcing brake
 * ! DON'T RUN THIS FOR TOO LONG !
 */
void GearMotor::brake() {
    ledcWrite(CH1, 0);
    ledcWrite(CH2, 0);
}
long GearMotor::get_encoder_count() {
    return m_encoder_count;
}

void GearMotor::reset_encoder_count() {
    m_encoder_count = 0;
}
