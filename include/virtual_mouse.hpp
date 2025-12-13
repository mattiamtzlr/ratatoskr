#pragma once
#include "mouse.hpp"

class VirtualMouse : public Mouse {
    virtual void moveForward(int distance = 1);
    virtual void moveForward(float distance);
    virtual void moveForwardHalf(int num_half_steps = 1);
    virtual void turn(int angle);

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual void update_visuals(Maze &maze);
    virtual void log(std::string msg);

    virtual void pause(int ms);
    virtual bool wasReset();
    virtual void ackReset();
};
