#include "mouse.hpp"

Mouse::Mouse(Maze &maze) : m_maze(maze) {}

//===============================[ MAZE ]=======================================
void Mouse::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, cardinalCharToDir[direction]);
}

void Mouse::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, cardinalCharToDir[direction]);
}

int Mouse::mazeHeight() { return m_maze.maze_height(); }
int Mouse::mazeWidth() { return m_maze.maze_width(); }

//===============================[ CONTROL ]====================================
/**
 * turn @angle radians in counterclockwise direction
 */
void Mouse::turn(int angle) {}
void Mouse::turnLeft() { turn(90); }
void Mouse::turnRight() { turn(-90); }

/**
 * move @distance [units] forwards
 */
void Mouse::moveForward(int distance) {}

//===============================[ SENSING ]====================================
bool Mouse::wallFront() {}
bool Mouse::wallRight() {}
bool Mouse::wallLeft() {}
