#include "mouse.hpp"
#include "maze.hpp"

Mouse::Mouse(Maze &maze) : m_maze(maze) {}

void Mouse::turnLeft() { turn(90); }

void Mouse::turnRight() { turn(-90); }

void Mouse::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, direction_to_cardinal(direction));
}

void Mouse::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, direction_to_cardinal(direction));
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
