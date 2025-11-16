#pragma once
#include <Arduino.h>

#include "gear_motor.hpp"
#include "gyro.hpp"
#include "led_matrix.hpp"
#include "maze.hpp"
#include "mouse.hpp"
#include "ToF.hpp"
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
    /*LEDMatrix &m_screen;*/

       public
       : Ratatoskr(GearMotor &motor_left, GearMotor &motor_right, ToF &tof_left,
                   ToF &tof_front_left, ToF &tof_front_right,
                   ToF &tof_right, MPU6050 &gyro/*, LEDMatrix &screen*/);

    // static const float PERIOD;  // TODO: we compute this by recording 360/T
    static const u_int8_t TURN_PWM = 150;  // TODO: we set this arbitrarily

    // static const float TIME_PER_CELL;  // TODO: how long rat goes 16 cm

    static const u_int8_t FORWARD_PWM = 170;  // TODO: we set this arbiturarily

    inline void stop();

    static const uint16_t FRONT_WALL_MM =
        110;  // front considered blocked if any front ToF < this
    static const uint16_t SIDE_WALL_MM =
        70;  // side considered blocked if side ToF < this

    virtual void moveForward(int distance = 1);
    virtual void turn(int angle);

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual void update_visuals(Maze &maze);

    virtual bool wasReset();
    virtual void ackReset();
};
