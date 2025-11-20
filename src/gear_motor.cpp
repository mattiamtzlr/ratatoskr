#include "gear_motor.hpp"

#define GEARING      31.5
#define ENCODERMULT  6

void IRAM_ATTR GearMotor::isr_trampoline(void *obj) {
    ((GearMotor *)obj)->encoder_interrupt();
}

/*
 * GearMotor class constructor
 */
GearMotor::GearMotor(int in1, int in2, int ch1, int ch2,
                     int encoder_pin_1, int encoder_pin_2,
                     int max_pwm,
                     int encoder_sign)
    : IN1(in1),
      IN2(in2),
      CH1(ch1),
      CH2(ch2),
      ENCODER_PIN_1(encoder_pin_1),
      ENCODER_PIN_2(encoder_pin_2),
      m_encoder_count(0),
      m_delta_time(0),
      m_t_last_i(0),
      m_max_pwm(max_pwm),
      m_encoder_sign(encoder_sign) {

    // Motor pins and PWM setup (mirrors Motors::setupMotor_) 
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    ledcSetup(CH1, MOTOR_FREQ, MOTOR_RES);
    ledcSetup(CH2, MOTOR_FREQ, MOTOR_RES);
    ledcAttachPin(IN1, CH1);
    ledcAttachPin(IN2, CH2);

    // Start stopped
    ledcWrite(CH1, 0);
    ledcWrite(CH2, 0);

    // Encoder pins (quadrature A/B) – interrupt will be attached on A
    pinMode(ENCODER_PIN_1, INPUT);
    pinMode(ENCODER_PIN_2, INPUT);
}

/**
 * Internal encoder ISR: executed on ENCODER_PIN_1 (channel A) rising edge.
 * Integrates the behaviour of updateEncoderLeft/Right from encoders.cpp
 * into the motor itself. 
 */
void IRAM_ATTR GearMotor::encoder_interrupt() {
    uint32_t t_curr_i = micros();
    if (m_t_last_i < t_curr_i) {
        m_delta_time = t_curr_i - m_t_last_i;
    }
    m_t_last_i = t_curr_i;

    // Quadrature direction logic (like encoders.cpp, but per motor)
    int a = digitalRead(ENCODER_PIN_1);
    int b = digitalRead(ENCODER_PIN_2);

    // On a rising edge of A, A should be HIGH; direction depends on B.
    // The sign convention can be flipped with m_encoder_sign so that
    // one GearMotor can behave like "left" and another like "right".
    if (a > b) {
        // Match encoders.cpp left/right pattern, scaled by encoder_sign 
        m_encoder_count -= m_encoder_sign;
    } else {
        m_encoder_count += m_encoder_sign;
    }
}

/**
 * Get the rpm of the motor
 */
int GearMotor::get_rpm() {
    // Avoid division by zero
    if (m_delta_time == 0) {
        return 0;
    }

    float revolutions = static_cast<float>(m_delta_time);  // µs
    revolutions = 1.0f / revolutions;                      // rev per µs
    revolutions *= 1000000.0f;                             // rev per sec
    revolutions *= 60.0f;                                  // rev per min
    revolutions /= GEARING;                                // account for gear ratio
    revolutions /= ENCODERMULT;                            // ticks per rotation

    return static_cast<int>(revolutions);
}

/**
 * Set signed speed for the motor.
 * Positive speed: CH1 active, CH2 low.
 * Negative speed: CH2 active, CH1 low.
 */
void GearMotor::setSpeed(int16_t speed) {
    speed = constrain(speed, -m_max_pwm, m_max_pwm);

    if (speed >= 0) {
        ledcWrite(CH1, speed);
        ledcWrite(CH2, 0);
    } else {
        ledcWrite(CH1, 0);
        ledcWrite(CH2, -speed);
    }
}

/**
 * Spin motor "clockwise" at given (PWM) speed.
 */
void GearMotor::spin_cw(int speed) {
    setSpeed(abs(speed));
}

/**
 * Spin motor "counter-clockwise" at given (PWM) speed.
 */
void GearMotor::spin_ccw(int speed) {
    setSpeed(-abs(speed));
}

/**
 * Stop motor by coasting (both channels low).
 */
void GearMotor::coast() {
    setSpeed(0);
}

/**
 * Active brake, (both channels driven HIGH). 
 * ! DON'T RUN THIS FOR TOO LONG !
 */
void GearMotor::brake() {
    ledcWrite(CH1, m_max_pwm);
    ledcWrite(CH2, m_max_pwm);
}

/**
 * Get encoder tick count
 */
long GearMotor::get_encoder_count() {
    return m_encoder_count;
}

/**
 * Reset encoder tick count
 */
void GearMotor::reset_encoder_count() {
    m_encoder_count = 0;
}
