#pragma once

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

    void bfs();
    void face(Direction target_dir);
    void detect_and_set_walls();

   public:
    void solve();
    void finalize_discovery();
    void run(std::vector<Position> solved,
             std::vector<std::vector<Position>> diagonals);
    std::vector<Position> bfs_shortest_path(Position start);
    std::vector<Position> dijkstra(Position start);
    Solver(Mouse &mouse, Maze &maze);
};
