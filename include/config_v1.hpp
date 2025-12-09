#pragma once

#include <cmath>
#include <vector>

#include "util.hpp"

namespace Config {
/* ================================== maze ================================== */
constexpr int MAZE_WIDTH = 7;
constexpr int MAZE_HEIGHT = 5;

const std::vector<Position> START_POINTS = {Position(0, 0)};
const std::vector<Position> END_POINTS = {Position(6, 4)};

constexpr int CELL_SIZE_MM = 180;
constexpr int WALL_TO_WALL_MM = 160;

/* ================================ movement ================================ */
constexpr int MIN_TURN_PWM = 185;
constexpr int MAX_TURN_PWM = 195;
constexpr int TURN_TIME_LIMIT = 1500;
constexpr float TURN_TRESHOLD = 0.2f;

constexpr u_int8_t FORWARD_PWM = 180;
constexpr int STOP_DISTANCE = 40;

/* =============================== dimensions =============================== */
constexpr u_int8_t WIDTH_MM = 70;
constexpr float MM_PER_CELL = 180.0;
constexpr float DIST_BETWEEN_SENSORS_MM = 46.0f;

/* =============================== motor stuff ============================== */
/*  one revolution is 31*pi mm = 97.4 mm */
constexpr u_int8_t WHEEL_DIAMETER_MM = 31;

constexpr uint8_t GEARING = 30;
constexpr uint8_t ENCODER_MULTIPLIER = 6;

/*  So encooder counts per mm = counts per rev / (WHEEL_DIAMETER_MM * pi) */
constexpr float ENCODER_COUNTS_PER_MM = 190.0f / (WHEEL_DIAMETER_MM * M_PI);

constexpr float PWM_UPDATE_RATIO = .6f;
constexpr float MAX_PWM_CORRECTION = 30.0f;

/* =============================== ToF stuff ================================ */
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

constexpr uint16_t TOF_SIDE_EXPECTED_MM = 50;     /* ----------------- */
constexpr uint16_t TOF_FRONT_EXPECTED_MM = 40;    /*   measured and    */
constexpr int16_t TOF_LEFT_OFFSET_MM = -33;       /*   calibrated in   */
constexpr int16_t TOF_FRONT_LEFT_OFFSET_MM = -18; /*   calibration     */
constexpr int16_t TOF_FRONT_RIGHT_OFFSET_MM = 2;  /*   cage (TM)       */
constexpr int16_t TOF_RIGHT_OFFSET_MM = -7;       /* ----------------- */
/* =============================== Diag stuff =============================== */
constexpr float MAX_DIST_MM = 100.0f;
}  // namespace Config
