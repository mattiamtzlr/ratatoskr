#include "mouse.hpp"
#include "maze.hpp"

void Mouse::turnLeft() {
    turn(90);
    dir = (Direction)((dir + 3) % 4);
}

void Mouse::turnRight() {
    turn(-90);
    dir = (Direction)((dir + 1) % 4);
}

void Mouse::moveForward(int distance) {
    pos.y += distance * (dir == NORTH) - (dir == SOUTH);
    pos.x += distance * (dir == EAST) - (dir == WEST);
}

void Mouse::setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
}

Position Mouse::getPosition() {
    return pos;
}
