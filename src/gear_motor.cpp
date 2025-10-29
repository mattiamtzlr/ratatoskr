#include "gear_motor.hpp"

#define GEARING \
    20  // TODO: These two constants may not be correct, test with actual
        // hardware
#define ENCODERMULT 12

/*
 * GearMotor class constructor
 */

GearMotor::GearMotor(int in1, int in2, int encoder_pin_1, int encoder_pin_2,
                     int max_pwm)
    : IN1(in1),
      IN2(in2),
      ENCODER_PIN_1(encoder_pin_1),
      ENCODER_PIN_2(encoder_pin_2) {

	m_actual_rpm = 0;
	m_desired_rpm = 0;
	m_t_last_i = 0;



    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(ENCODER_PIN_1, INPUT_PULLUP);
    pinMode(ENCODER_PIN_2, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_1), encoder_interrupt,
                    RISING);
}

/**
 * private method; executed whenever `ENCODER_PIN_1` input signal
 * changes
 */
void IRAM_ATTR GearMotor::encoder_interrupt() {
    // This function is heavily inspired from
    // https://www.adafruit.com/product/4640
    int t_curr_i = micros();
    if (m_t_last_i < t_curr_i) {
        // did not wrap around
        float rev = t_curr_i - m_t_last_i;  // us
        rev = 1.0 / rev;                    // rev per us
        rev *= 1000000;                     // rev per sec
        rev *= 60;                          // rev per min
        rev /= GEARING;                     // account for gear ratio
        rev /= ENCODERMULT;  // account for multiple ticks per rotation
        m_actual_rpm = rev;
    }
    m_t_last_i = t_curr_i;
}

/**
 * Get the rpm of the motor
 */
int GearMotor::get_rpm() { return m_actual_rpm; }

/**
 * private method; sets rpm of specified pin.errorhandling
 */

void GearMotor::set_rpm(int pin, int rpm) {
    m_desired_rpm = rpm;
    int pwm = (MIN_PWM + (MAX_PWM - MIN_PWM) * m_desired_rpm / MAX_RPM);
    analogWrite(pin, pwm);
}

/**
 * Spin motor clockwise
 */
void GearMotor::spin_cw(int rpm) {
    digitalWrite(IN1, LOW);
    set_rpm(IN2, rpm);
}

/**
 * Spin motor counter-clockwise
 */
void GearMotor::spin_ccw(int rpm) {
    digitalWrite(IN2, LOW);
    set_rpm(IN1, rpm);
}

/**
 * Stop motor
 */
void GearMotor::stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}
