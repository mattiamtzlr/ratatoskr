#pragma once
#include <string>
#include <set>
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

    virtual void turn(int angle) = 0;

    virtual bool wallFront() = 0;
    virtual bool wallRight() = 0;
    virtual bool wallLeft() = 0;

    virtual void setWall(int x, int y, char direction) = 0;
    virtual void clearWall(int x, int y, char direction) = 0;

    virtual void setColor(int x, int y, char color) = 0;
    virtual void clearColor(int x, int y) = 0;
    virtual void clearAllColor() = 0;

    virtual void setText(int x, int y, const std::string& text) = 0;
    virtual void clearText(int x, int y) = 0;
    virtual void clearAllText() = 0;

    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;
    virtual void update_visuals(Maze &maze) = 0;
};
