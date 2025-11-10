#pragma once
#include "maze.hpp"
#include "util.hpp"

class Mouse {
  private:
    Maze &m_maze;
    Position pos = Position(0,0);

  public:
    Mouse(Maze &maze);

    void turnLeft();
    void turnRight();

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    int mazeWidth();
    int mazeHeight();

    void setPosition(int x, int y);
    Position getPosition();

    virtual void moveForward(int distance = 1) = 0;
    virtual void turn(int angle) = 0;
    

    virtual bool wallFront() = 0;
    virtual bool wallRight() = 0;
    virtual bool wallLeft() = 0;

    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;

    virtual ~Mouse() = 0;
};
