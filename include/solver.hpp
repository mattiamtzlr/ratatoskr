#pragma once

#include "mouse.hpp"
#include "maze.hpp"

class Solver {
   private:
    Mouse &m_mouse;
    Maze &m_maze;

   public:
    void solve();
    Solver(Mouse &mouse, Maze &maze);

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    int mazeWidth();
    int mazeHeight();
};
