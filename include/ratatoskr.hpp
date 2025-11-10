#pragma once
#include "gear_motor.hpp"
#include "gyro.hpp"
#include "led_matrix.hpp"
#include "maze.hpp"
#include "tof.hpp"
#include "util.hpp"
#include "mouse.hpp"

class Ratatoskr : public Mouse {
   private:
    GearMotor &m_motor_left;
    GearMotor &m_motor_right;
    ToF &m_tof_left;
    ToF &m_tof_front_left;
    ToF &m_tof_front_right;
    ToF &m_tof_right;
    MPU6050 &m_gyro;
    LEDMatrix &m_screen;

   public:
    Ratatoskr(Maze &maze, GearMotor &motor_right, GearMotor &motor_left,
              ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
              ToF &tof_right, MPU6050 &gyro, LEDMatrix &screen);

    virtual void moveForward(int distance = 1);
    virtual void turn(int angle);

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual bool wasReset();
    virtual void ackReset();
};
