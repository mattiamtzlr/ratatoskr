#pragma once

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
static constexpr int MIN_TURN_PWM = 185;
static constexpr int MAX_TURN_PWM = 190;
static constexpr int TURN_TIME_LIMIT = 1500;
static constexpr float TURN_TRESHOLD = 0.2f;

static constexpr uint8_t FORWARD_PWM = 180;
static constexpr int STOP_DISTANCE = 40;


/* =============================== dimensions =============================== */
static constexpr uint8_t WIDTH_MM = 70;


/* =============================== motor stuff ============================== */
static constexpr uint8_t GEARING = 30;
static constexpr uint8_t ENCODER_MULTIPLIER = 6;

/* measured by pushing the mouse exactly one cell length */
static constexpr uint16_t ENCODER_COUNTS_PER_CELL = 255;

/*  encoder counts per mm = counts per cell / cell size */
static constexpr float ENCODER_COUNTS_PER_MM =
    static_cast<float>(ENCODER_COUNTS_PER_CELL) /
    static_cast<float>(CELL_SIZE_MM);

static constexpr float PWM_UPDATE_RATIO = .6f;
static constexpr float MAX_PWM_CORRECTION = 30.0f;


/* =============================== ToF stuff ================================ */
static constexpr uint8_t TOF_LEFT_ADDRESS = 0x30;
static constexpr uint8_t TOF_FRONT_LEFT_ADDRESS = 0x31;
static constexpr uint8_t TOF_FRONT_RIGHT_ADDRESS = 0x32;
static constexpr uint8_t TOF_RIGHT_ADDRESS = 0x33;

/*  beyond this: treat as "no wall" */
static constexpr uint16_t MAX_TOF_VALID_MM = 120;
/*  front considered blocked if any front ToF < this */
static constexpr uint16_t FRONT_WALL_MM = 110;
/*  side considered blocked if side ToF < this */
static constexpr uint16_t SIDE_WALL_MM = 90;

static constexpr uint16_t TOF_SIDE_EXPECTED_MM = 50;      /* --------------- */
static constexpr uint16_t TOF_FRONT_EXPECTED_MM = 40;     /*  measured and   */
static constexpr int16_t TOF_LEFT_OFFSET_MM = -49;        /*  calibrated in  */
static constexpr int16_t TOF_FRONT_LEFT_OFFSET_MM = -17;  /*  calibration    */
static constexpr int16_t TOF_FRONT_RIGHT_OFFSET_MM = -59; /*  cage (TM)      */
static constexpr int16_t TOF_RIGHT_OFFSET_MM = -7;        /* --------------- */
}
