#include "gear_motor.hpp"
#include "esp32-hal.h"

#include <Arduino.h>

/*
 * GearMotor class constructor
 */

GearMotor::GearMotor(int in1, int in2, int encoder_pin_1, int encoder_pin_2, int max_pwm)
    : IN1(in1), IN2(in2), ENCODER_PIN_1(encoder_pin_1), ENCODER_PIN_2(encoder_pin_2) {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(ENCODER_PIN_1, INPUT_PULLUP);
    pinMode(ENCODER_PIN_2, INPUT_PULLUP);
    // TODO: finish this? i.e., handle interrupts, etc
}

/**
 * Get the rpm of the motor
 */

int GearMotor::get_rpm() { return m_desired_rpm; } // TODO: read encoder values!

/**
 * private method; sets rpm of specified pin.
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
