#include "pid.hpp"

#include <Arduino.h>

PID::PID(float time_step, float k_p, float k_i, float k_d, float lower, float upper)
    : TIME_STEP(time_step), k_p(k_p), k_i(k_i), k_d(k_d), LOWER_BOUND(lower), UPPER_BOUND(upper) {
    reset();
}

float PID::update(float error) {
    integral += error * TIME_STEP;

    float derivative = (error - previous_error) / TIME_STEP;
    previous_error = error;

    corrected_signal = k_p * error + k_i * integral + k_d * derivative;

    corrected_signal = constrain(corrected_signal, LOWER_BOUND, UPPER_BOUND);

    return corrected_signal;
}

void PID::reset() {
    previous_error = 0;
    integral = 0;
}
