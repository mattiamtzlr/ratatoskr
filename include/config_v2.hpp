#pragma once

#include <cstdint>
#include <vector>

#include "util.hpp"

namespace Config {
/* ========================================================================== */
/* |                                  maze                                  | */
/* ========================================================================== */
constexpr int MAZE_WIDTH = 3;
constexpr int MAZE_HEIGHT = 5;

const std::vector<Position> START_POINTS = {Position(0, 0)};
const std::vector<Position> END_POINTS = {Position(2, 4)};

constexpr int CELL_SIZE_MM = 180;
constexpr int WALL_TO_WALL_MM = 160;

/* ========================================================================== */
/* |                                movement                                | */
/* ========================================================================== */
constexpr int TURN_PWM = 185;
constexpr int TURN_TIME_LIMIT = 1500;
constexpr float TURN_TRESHOLD = 3.f;

constexpr uint8_t FORWARD_PWM = 181;
constexpr uint8_t FORWARD_FAST_PWM_CHUNK = 10;
constexpr uint8_t FORWARD_FAST_PWM_MAX = 240;
constexpr float FORWARD_OVERSHOOT_SLOPE = 0.02f;
constexpr int STOP_DISTANCE = 55;
/* ========================================================================== */
/* |                               dimensions                               | */
/* ========================================================================== */
constexpr uint8_t WIDTH_MM = 70;
constexpr float DIST_BETWEEN_SENSORS_MM = 53.75f;

/* ========================================================================== */
/* |                               motor stuff                              | */
/* ========================================================================== */
constexpr uint8_t GEARING = 30;
constexpr uint8_t ENCODER_MULTIPLIER = 6;

/* measured by pushing the mouse exactly one cell length */
constexpr uint16_t ENCODER_COUNTS_PER_CELL = 265;

/*  encoder counts per mm = counts per cell / cell size */
constexpr float ENCODER_COUNTS_PER_MM =
    static_cast<float>(ENCODER_COUNTS_PER_CELL) /
    static_cast<float>(CELL_SIZE_MM);

constexpr float PWM_UPDATE_RATIO = .6f;
constexpr float MAX_PWM_CORRECTION = 30.0f;

/* ========================================================================== */
/* |                                ToF stuff                               | */
/* ========================================================================== */
constexpr uint8_t TOF_LEFT_ADDRESS = 0x30;
constexpr uint8_t TOF_FRONT_LEFT_ADDRESS = 0x31;
constexpr uint8_t TOF_FRONT_RIGHT_ADDRESS = 0x32;
constexpr uint8_t TOF_RIGHT_ADDRESS = 0x33;

/*  beyond this: treat as "no wall" */
constexpr uint16_t MAX_TOF_VALID_MM = 120;
/*  front considered blocked if any front ToF < this */
constexpr uint16_t FRONT_WALL_MM = 110;
/*  side considered blocked if side ToF < this */
constexpr uint16_t SIDE_WALL_MM = 90;

constexpr uint16_t TOF_SIDE_EXPECTED_MM = 50;      /* ----------------- */
constexpr uint16_t TOF_FRONT_EXPECTED_MM = 40;     /* | measured and  | */
constexpr int16_t TOF_LEFT_OFFSET_MM = -49;        /* | calibrated in | */
constexpr int16_t TOF_FRONT_LEFT_OFFSET_MM = -17;  /* | calibration   | */
constexpr int16_t TOF_FRONT_RIGHT_OFFSET_MM = -59; /* | cage (TM)     | */
constexpr int16_t TOF_RIGHT_OFFSET_MM = -7;        /* ----------------- */

/* ========================================================================== */
/* |                                diagonals                               | */
/* ========================================================================== */
const float MAX_DIST_MM = 150.0f;
}  // namespace Config
