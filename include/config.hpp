#pragma once

#include <Arduino.h>

namespace Config {
    static const int MIN_TURN_PWM = 185;
    static const int MAX_TURN_PWM = 195;

    static const int STOP_DISTANCE = 40;

    static const u_int8_t FORWARD_PWM = 190;

    static const u_int8_t WIDTH_MM = 68;

    /*  So one revolution is 31*pi mm = 97.4 mm */
    static const u_int8_t WHEEL_DIAMETER_MM = 31;

    /*  So encooder counts per mm = counts per rev / (WHEEL_DIAMETER_MM * pi) */
    static constexpr float ENCODER_COUNTS_PER_MM =
        190.0f / (WHEEL_DIAMETER_MM * PI);

    static constexpr float MM_PER_CELL = 180.0;

    static constexpr float PWM_UPDATE_RATIO = .6f;

    /*  front considered blocked if any front ToF < this */
    static const uint16_t FRONT_WALL_MM = 110;

    /*  side considered blocked if side ToF < this */
    static const uint16_t SIDE_WALL_MM = 90;

    static constexpr float MAX_PWM_CORRECTION = 30.0f;
    static constexpr uint16_t TARGET_SIDE_MM =
        50; /*  desired side distance at sensor */
    static constexpr uint16_t MAX_TOF_VALID_MM =
        120; /*  beyond this: treat as "no wall" */

    static constexpr int TURN_TIME_LIMIT = 1500;
    static constexpr float TURN_TRESHOLD = 0.2f;
}