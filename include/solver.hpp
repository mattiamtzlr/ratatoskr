#pragma once

#include <deque>
#include <set>
#include <vector>

#include "maze.hpp"
#include "mouse.hpp"
#include "util.hpp"

class Solver {
   private:
    Mouse &m_mouse;
    Maze &m_maze;

    int width;
    int height;

    int UBOUND_DIST;



    void set_wall(Position pos, Direction d);
    void bfs_recompute();

    std::vector<std::pair<int, int>> compute_blue_route();

    void face(Direction target_dir);

    void move_forward();

    // Sense & log
    void detect_and_set_walls();

   public:
    void solve();
    Solver(Mouse &mouse, Maze &maze);
};
