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
    /*
    * Turn left 90°, update the direction of the mouse
    */
    void turnLeft();
    /*
    * Turn right 90°, update the direction of the mouse
    */
    void turnRight();
    /*
    * Turn right 45°, update the direction of the mouse
    */
    void turnRight45();
    /*
    * Turn left 45°, update the direction of the mouse
    */
    void turnLeft45();
    /*
    * Turn left or right 180° (depending on where there is more space), 
    * update the direction of the mouse
    */
    void turn180();

    /*
    * Update the position of the mouse on `x,y`
    */
    void setPosition(int x, int y);
    /*
    * Return the current position of the mouse 
    */
    Position getPosition();
    /*
    * Get the current direction of the mouse
    */
    Direction getDirection();

    bool is_in_diagonal();

    int mazeWidth();
    int mazeHeight();

    // Call this method inside the overriding method in the derived class!

    virtual void moveForward(float distance) = 0;
    virtual void moveForward(int distance = 1) = 0;
    virtual void moveForwardHalf(int num_half_steps = 1) = 0;

    virtual void turn(int angle) = 0;

    virtual bool wallFront() = 0;
    virtual bool wallRight() = 0;
    virtual bool wallLeft() = 0;

    virtual void update_visuals(Maze &maze) = 0;
    virtual void log(std::string msg) = 0;
    
    virtual void pause(int ms) = 0;
    virtual bool wasReset() = 0;
    virtual void ackReset() = 0;
};
