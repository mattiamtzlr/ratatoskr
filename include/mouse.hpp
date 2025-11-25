#pragma once
#include <set>
#include <string>

#include "maze.hpp"
#include "util.hpp"

class Mouse {
   private:
    Position pos = Position(0, 0);
    Direction dir = NORTH;

   public:
    void turnLeft();
    void turnRight();

    void setPosition(int x, int y);
    Position getPosition();
    Direction getDirection();

    int mazeWidth();
    int mazeHeight();

    // Call this method inside the overriding method in the derived class!
    virtual void moveForward(int distance = 1);
    virtual void moveForwardHalf(int num_half_steps = 1);

    virtual void turn(int angle) = 0;

    virtual bool wallFront() = 0;
    virtual bool wallRight() = 0;
    virtual bool wallLeft() = 0;

    virtual void update_visuals(Maze &maze) = 0;

    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;
};
