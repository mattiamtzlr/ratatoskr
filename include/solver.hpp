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
    std::vector<Instruction> parse_path(std::vector<GraphCoordinate> path);
    std::vector<Position> bfs_shortest_path(Position start);
    std::vector<GraphCoordinate> dijkstra(GraphCoordinate start);
    Solver(Mouse &mouse, Maze &maze);
};
