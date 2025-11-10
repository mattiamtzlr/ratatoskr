#include "mouse.hpp"
#include "maze.hpp"

Mouse::Mouse(Maze &maze) : m_maze(maze) {}

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

void Mouse::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, cardinalCharToDir[direction]);
}

void Mouse::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, cardinalCharToDir[direction]);
}

int Mouse::mazeHeight() {
    return m_maze.maze_height();
}

int Mouse::mazeWidth() {
    return m_maze.maze_width();
}

void Mouse::setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
}

Position Mouse::getPosition() {
    return pos;
}
