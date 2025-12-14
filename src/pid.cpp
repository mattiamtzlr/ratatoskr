#include "pid.hpp"

#include <Arduino.h>

PID::PID(float k_p, float k_i, float k_d) : k_p(k_p), k_i(k_i), k_d(k_d) {
    reset();
}

float PID::update(float time_step, float error) {
    /* Simple Riemann sum */
    integral += error * time_step;

    /* Approximative derivative */
    float derivative = (error - previous_error) / time_step;

    previous_error = error;

    return k_p * error + k_i * integral + k_d * derivative;
}

void PID::reset() {
    previous_error = 0;
    integral = 0;
}
