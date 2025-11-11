#pragma once
#include "mouse.hpp"

class VirtualMouse : public Mouse {

    virtual void moveForward(int distance = 1);
    virtual void turn(int angle);

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual void setWall(int x, int y, char direction);
    virtual void clearWall(int x, int y, char direction);

    virtual void setColor(int x, int y, char color);
    virtual void clearColor(int x, int y);
    virtual void clearAllColor();

    virtual void setText(int x, int y, const std::string& text);
    virtual void clearText(int x, int y);
    virtual void clearAllText();

    virtual bool wasReset();
    virtual void ackReset();
};
