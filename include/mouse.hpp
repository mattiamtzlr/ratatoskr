#pragma once
#include "tof.hpp"
#include "gear_motor.hpp"
#include "gyro.hpp"
#include "maze.hpp"
#include "led_matrix.hpp"
#include "util.hpp"

class Mouse {
   private:
    Maze &m_maze;
    GearMotor motor_left;
    GearMotor motor_right;
    ToF tof_left;
    ToF tof_front_left;
    ToF tof_front_right;
    ToF tof_right;
    MPU6050 gyro;
   LEDMatrix screen;

       public : Mouse(Maze &maze);

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
