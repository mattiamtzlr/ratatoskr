#include "mouse.hpp"

#include "maze.hpp"
#include "util.hpp"

int Mouse::mazeWidth() { return MAZE_WIDTH; }
int Mouse::mazeHeight() { return MAZE_HEIGHT; }

void Mouse::turnLeft() {
    turn(90);
    dir = rotate_left(dir);
}

void Mouse::turnRight() {
    turn(-90);
    dir = rotate_right(dir);
}

void Mouse::moveForward(int distance) {
    pos.y += distance * ((dir == NORTH) - (dir == SOUTH));
    pos.x += distance * ((dir == EAST) - (dir == WEST));
}

void Mouse::moveForwardHalf(int num_half_steps) {
    pos.y += num_half_steps * 0.5 * ((dir == NORTH) - (dir == SOUTH));
    pos.x += num_half_steps * 0.5 * ((dir == EAST) - (dir == WEST));
}

void Mouse::setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
}

Position Mouse::getPosition() { return pos; }

Direction Mouse::getDirection() { return dir; }
