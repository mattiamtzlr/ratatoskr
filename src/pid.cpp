#include "pid.hpp"

#include <Arduino.h>

PID::PID(float k_p, float k_i, float k_d) : k_p(k_p), k_i(k_i), k_d(k_d) {
    reset();
}

/**
 * Update function for the PID.
 *
 * @param error and time since the last update.
 * @return the corrected signal.
 */
float PID::update(float time_step, float error) {
    integral += error * time_step;
    float derivative = (error - previous_error) / time_step;
    previous_error = error;

    return k_p * error + k_i * integral + k_d * derivative;
}

/**
 * Reset the PID.
 *
 * Will set the integral and the previous error to zero.
 *
 */
void PID::reset() {
    previous_error = 0;
    integral = 0;
}
