#include "solver.hpp"

Solver::Solver(Mouse &mouse, Maze &maze) : m_mouse(mouse), m_maze(maze) {}

void Solver::solve() {
    //m_mouse.setup();
    while (!m_maze.at_target(m_mouse.getPosition())) {
        //detect_walls();
        if (!m_mouse.wallLeft()) {
            m_mouse.turnLeft();
        }
        while (m_mouse.wallFront()) {
            m_mouse.turnRight();
        }
        m_mouse.moveForward();
    }
}

void Solver::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, cardinalCharToDir[direction]);
}

void Solver::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, cardinalCharToDir[direction]);
}

int Solver::mazeHeight() {
    return m_maze.maze_height();
}

int Solver::mazeWidth() {
    return m_maze.maze_width();
}

