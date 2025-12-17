#pragma once
#include "mouse.hpp"

class VirtualMouse : public Mouse {
    /*
    * Move the mouse forward by `distance` cells
    */
    virtual void moveForward(int distance = 1);
    /*
    * Move the mouse forward by `distance` cells (can be fractional)
    */
    virtual void moveForward(float distance);
    /*
    * Move the mouse by half cell steps (needed for diagonal movement)
    */
    virtual void moveForwardHalf(int num_half_steps = 1);
    /*
    * Turn the mouse by `angle` degrees
    */
    virtual void turn(int angle);

    /*
    * Check if there is a wall in front of the mouse
    */
    virtual bool wallFront();
    /*
    * Check if there is a wall to the right of the mouse
    */
    virtual bool wallRight();
    /*
    * Check if there is a wall to the left of the mouse
    */
    virtual bool wallLeft();
    /*
    * Update the visuals of the maze and mouse (only in the MMS simulator)
    */
    virtual void update_visuals(Maze &maze);
    /*
    * Write a log message (only in the MMS simulator)
    */
    virtual void log(std::string msg);

    /*
    * Pause execution for `s` seconds
    */
    virtual void pause(int s);
    virtual bool wasReset();
    virtual void ackReset();
};
