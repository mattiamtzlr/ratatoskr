#include "pid.hpp"

#include <Arduino.h>

#include "ratatoskr.hpp"

PID::PID()
    : distanceKp(0.25),
      distanceKi(0.1),
      distanceKd(0.00),
      encoderKp(0.75),
      encoderKi(0.80),
      encoderKd(0.10),
      distancePrevError(0),
      encoderPrevError(0),
      distanceIntegral(0),
      encoderIntegral(0),
      basePWM_forward(Ratatoskr::FORWARD_PWM) {}

std::pair<int, int> PID::update(float time_step, float leftDistance,
                                float rightDistance, float leftEncoder,
                                float rightEncoder) {
    // Constants
    const float targetPosition = 0.0;

    leftDistance =
        (leftDistance == 0 && rightDistance > 0) ? 0.75 : leftDistance;

    rightDistance =
        (rightDistance == 0 && leftDistance > 0) ? 0.75 : rightDistance;

    // Lateral position error (positive = too far right, negative = too far
    // left)
    float leftConstrained = constrain(leftDistance, 0, 80);
    float rightConstrained = constrain(rightDistance, 0, 80);
    float currentPosition = leftConstrained - rightConstrained;

    // PID for lateral control
    float distanceError = targetPosition - currentPosition;
    distanceIntegral += distanceError * time_step;
    float distanceDerivative = (distanceError - distancePrevError) / time_step;
    distancePrevError = distanceError;

    float side_correction = distanceKp * distanceError +
                            distanceKi * distanceIntegral +
                            distanceKd * distanceDerivative;

    // PID for encoder synchronization (keep wheels matched) (think of plane
    // wings)
    float targetEncoder = 0.0;
    float currentEncoder = leftEncoder - rightEncoder;
    float encoderError = targetEncoder - currentEncoder;
    encoderIntegral += encoderError * time_step;
    float encoderDerivative = (encoderError - encoderPrevError) / time_step;

    float encoderCorrection = encoderKp * encoderError +
                              encoderKi * encoderIntegral +
                              encoderKd * encoderDerivative;
    encoderPrevError = encoderError;

    // Disable lateral correction if both walls are far or absent
    // if ((leftDistance >= 40 || leftDistance == 0) &&
    //     (rightDistance >= 40 || rightDistance == 0)) {
    //     side_correction = 0;
    // }

    // Calculate final PWM values
    corrected_left_PWM = basePWM_forward + side_correction + encoderCorrection;
    corrected_right_PWM = basePWM_forward - side_correction - encoderCorrection;

    return std::make_pair(constrain(corrected_left_PWM, 40, 255),
                          constrain(corrected_right_PWM, 40, 255));
}

void PID::reset() {
    distancePrevError = 0;
    encoderPrevError = 0;
    distanceIntegral = 0;
    encoderIntegral = 0;
}
