#pragma once
#include "gear_motor.hpp"
#include "gyro.hpp"
#include "led_matrix.hpp"
#include "maze.hpp"
#include "tof.hpp"
#include "util.hpp"

class Mouse {
   private:
    Maze &m_maze;
    GearMotor &m_motor_left;
    GearMotor &m_motor_right;
    ToF &m_tof_left;
    ToF &m_tof_front_left;
    ToF &m_tof_front_right;
    ToF &m_tof_right;
    MPU6050 &m_gyro;
    LEDMatrix &m_screen;

   public:
    Mouse(Maze &maze, GearMotor &motor_right, GearMotor &motor_left,
          ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
          ToF &tof_right, MPU6050 &gyro, LEDMatrix &screen);

    void turnLeft();
    void turnRight();

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    int mazeWidth();
    int mazeHeight();

    virtual void moveForward(int distance = 1) = 0;
    virtual void turn(int angle) = 0;

    virtual bool wallFront() = 0;
    virtual bool wallRight() = 0;
    virtual bool wallLeft() = 0;

    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;

    virtual ~Mouse() = 0;
};
