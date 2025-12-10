#pragma once
#include <Arduino.h>

#include <cstdint>

#include "ToF.hpp"
#include "config.hpp"
#include "esp_logger.hpp"
#include "gear_motor.hpp"
#include "gyro.hpp"
#include "maze.hpp"
#include "mouse.hpp"
#include "oled.hpp"
#include "pid.hpp"
#include "util.hpp"

class Ratatoskr : public Mouse {
   private:
    GearMotor &m_motor_left;
    GearMotor &m_motor_right;
    ToF &m_tof_left;
    ToF &m_tof_front_left;
    ToF &m_tof_front_right;
    ToF &m_tof_right;
    MPU6050 &m_gyro;
    OLED &m_oled;

    bool too_close_front(uint16_t front_left_dist, uint16_t front_right_dist);
    inline void stop();
    inline void coast();
    inline void safe_stop();

    PID m_pid_encoders{0.75, 0.8, 0.1};
    PID m_pid_tof_sides{0.5, 0.0, 0.0};
    PID m_pid_tof_front_diagonals{1., 0.0, 0.0};

   public:
    Ratatoskr(GearMotor &motor_left, GearMotor &motor_right, ToF &tof_left,
              ToF &tof_front_left, ToF &tof_front_right, ToF &tof_right,
              MPU6050 &gyro, OLED &oled);

    /**
     * move @distance cells forward diagonally with PID control
     */
    void moveDiagonal(float distance);

    /**
     * move @distance cells forward with PID control
     */
    virtual void moveForward(int distance = 1);
    void moveForward(float distance);
    void moveForwardHalf(int num_half_steps = 1);

    /**
     * turn @angle degrees in counterclockwise direction
     */
    virtual void turn(int angle);

    /**
     * Front wall if either front-left or front-right ToF sees something closer
     * than FRONT_WALL_MM. Ignores zero readings (sensor not ready).
     */
    virtual bool wallFront();

    /**
     * Detect if a wall is towards the right.
     */
    virtual bool wallRight();

    /**
     * Detect if a wall is towards the left.
     */
    virtual bool wallLeft();

    void update_screen(float gyro_angle, Face face = NEUTRAL);
    virtual void update_visuals(Maze &maze);
    virtual void log(std::string msg);

    virtual bool wasReset();
    virtual void ackReset();
};
