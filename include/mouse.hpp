#pragma once
#include "maze.hpp"

class Mouse {
  private:
    Maze &m_maze;
  public:
    Mouse(Maze &maze);

    bool wallFront();
    bool wallRight();
    bool wallLeft();

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    void turnLeft() { turn(90); }
    void turnRight() { turn(-90); }

    virtual void moveForward(int distance = 1) = 0;
    virtual void turn(int angle) = 0;

    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;


    virtual ~Mouse() = 0;
};
