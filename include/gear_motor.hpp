#pragma once
#include <Arduino.h>
#include "util.hpp"

// TODO: This is no longer the one we use : [Adafruit DRV8871 DC Motor Driver
// Breakout Board]
class GearMotor {
   public:
    GearMotor(int pin_1, int pin_2, int encoder_pin_1, int encoder_pin_2,
              int max_pwm);

    void spin_cw(int speed);
    void spin_ccw(int speed);
    void stop();
    int get_rpm();
    void encoder_interrupt();
    static void isr_trampoline(void *obj_isr);

    // Encoder cumulative count utilities
    long get_encoder_count();
    void reset_encoder_count();

   private:
    void set_rpm(int pin, int speed);

    static constexpr int MAX_PWM =
        255;  // TODO: Change this before running on real hardware!
    static constexpr int MIN_PWM =
        40;  // TODO: Change this before running on real hardware!
    static constexpr int MAX_RPM =
        500;  // TODO: Change this before running on real hardware!

    const int IN1;
    const int IN2;
    const int ENCODER_PIN_1;
    const int ENCODER_PIN_2;
    int m_desired_rpm;
    volatile int m_delta_time;
    volatile int m_t_last_i;
    volatile int m_encoder_t_diff;
    volatile long m_encoder_count = 0;
};
