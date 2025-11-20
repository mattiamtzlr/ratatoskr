#pragma once

#include <Arduino.h>

constexpr int MOTOR_FREQ = 20000;  // 20 kHz
constexpr int MOTOR_RES  = 8;      // 8-bit resolution

class GearMotor {
public:
    // Trampoline used to attach the encoder ISR with a void* argument
    static void IRAM_ATTR isr_trampoline(void *obj);

    /*
     * GearMotor class constructor
     *
     * in1, in2       : H-bridge input pins
     * ch1, ch2       : LEDC PWM channels for in1/in2
     * encoder_pin_1  : encoder channel A (interrupt source)
     * encoder_pin_2  : encoder channel B
     * max_pwm        : maximum PWM value (e.g. 255 for 8-bit)
     * encoder_sign   : +1 or -1 to match physical encoder direction
     */
    GearMotor(int in1, int in2, int ch1, int ch2,
              int encoder_pin_1, int encoder_pin_2,
              int max_pwm = 255,
              int encoder_sign = 1);

    // --- Motion control (Motors-style API for a single motor) ---

    /// Set signed speed: positive = one direction, negative = opposite.
    /// Range is constrained to [-max_pwm, +max_pwm].
    void setSpeed(int16_t speed);

    /// Convenience: spin "clockwise" = positive speed
    void spin_cw(int speed);

    /// Convenience: spin "counter-clockwise" = negative speed
    void spin_ccw(int speed);

    /// Stop motor (coast): both PWM channels = 0
    void stop();

    /// Active brake: both channels driven HIGH (max_pwm)
    /// ! DON'T RUN THIS FOR TOO LONG !
    void brake();

    // --- Encoder / RPM measurement ---

    /// Executed on encoder_pin_1 (channel A) rising edge (attach this as ISR).
    void IRAM_ATTR encoder_interrupt();

    /// Get the RPM of the motor (based on last encoder period)
    int get_rpm();

    /// Get total encoder tick count (signed, includes direction)
    long get_encoder_count();

    /// Reset encoder tick count to zero
    void reset_encoder_count();

    /// Accessor for configured max PWM
    int16_t maxPwm() const { return max_pwm_; }

private:
    // H-bridge pins and PWM channels
    const int IN1;
    const int IN2;
    const int CH1;
    const int CH2;

    // Encoder pins (quadrature A/B)
    const int ENCODER_PIN_1;  // channel A
    const int ENCODER_PIN_2;  // channel B

    // Encoder state
    volatile long     m_encoder_count;
    volatile uint32_t m_delta_time;  // time between encoder pulses (µs)
    volatile uint32_t m_t_last_i;    // last interrupt timestamp (µs)

    // PWM limit
    int16_t max_pwm_;

    // Direction convention for this motor (+1 or -1)
    int m_encoder_sign;
};
