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
    int x = 0, y = 0, heading = NORTH;

    const int INF = height * width + 1;

    // walls[x][y][d], known[x][y][d], dist[x][y]
    std::vector<std::vector<std::array<bool, 4>>> walls;
    std::vector<std::vector<std::array<bool, 4>>> known;
    std::vector<std::vector<int>> dist;

    std::vector<std::pair<int, int>> goals;
    std::set<std::pair<int, int>> visited;

    char dir_char(int d);
    char dir_letter(int d);
    int dx(int d);
    int dy(int d);
    int opposite(int d);
    const char *bstr(bool v);

    bool in_bounds(int x, int y);
    void set_wall(int x, int y, int d, bool is_wall);
    bool is_goal(int x, int y);
    void recompute();
    long long key(int a, int b);

    std::vector<std::pair<int, int>> compute_blue_route(int sx, int sy);

    // Paint overlays
    void paint_colors(const std::set<std::pair<int, int>> &visited,
                      const std::vector<std::pair<int, int>> &blue);

    // Movement helpers with logs
    int turn_left();
    int turn_right();

    int face(int target_dir, int h);

    void move_forward(int &cx, int &cy, int h);

    // Sense & log
    void sense(int cx, int cy, int h);

   public:
    void solve();
    Solver(Mouse &mouse, Maze &maze);

    void setWall(int x, int y, char direction);
    void clearWall(int x, int y, char direction);

    int mazeWidth();
    int mazeHeight();
};
