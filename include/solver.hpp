#pragma once

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "API.hpp"
#include "maze.hpp"
#include "mouse.hpp"
#include "util.hpp"

class Solver {
   private:
    Mouse &m_mouse;
    Maze &m_maze;

    int width;
    int height;
    int x = 0;
    int y = 0;

    const int INF = height * width + 1;


    std::set<std::pair<int, int>> visited;

    void set_wall(Position pos, Direction d);
    void bfs_recompute();
    void update_text();

    std::vector<std::pair<int, int>> compute_blue_route(int sx, int sy);

    // Paint overlays
    void paint_colors(const std::set<std::pair<int, int>> &visited,
                      const std::vector<std::pair<int, int>> &blue);

    void face(Direction target_dir);

    void move_forward();

    // Sense & log
    void detect_and_set_walls();

   public:
    void solve();
    Solver(Mouse &mouse, Maze &maze);

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    int mazeWidth();
    int mazeHeight();
};
