#pragma once
#include "mouse.hpp"

class VirtualMouse : public Mouse {
    virtual void moveForward(int distance = 1);
    virtual void turn(int angle);

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual void update_visuals(Maze &maze);
    virtual void log(std::string msg);

    virtual bool wasReset();
    virtual void ackReset();
};
