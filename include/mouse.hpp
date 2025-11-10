#pragma once
#include "maze.hpp"

class Mouse {
  private:
    Maze &m_maze;

  public:
    Mouse(Maze &maze);

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
