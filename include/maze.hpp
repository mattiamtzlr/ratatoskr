#pragma once

#include <cstddef>
#include <set>

#include "util.hpp"

constexpr int MAZE_WIDTH = 16;
constexpr int MAZE_HEIGHT = 16;

class Maze {
   private:
    int m_wall_storage[MAZE_HEIGHT][MAZE_WIDTH] = {};
    int m_distances[MAZE_HEIGHT][MAZE_WIDTH] = {};

   public:
    Position targets[4] = {
        {7, 7},
        {8, 7},
        {7, 8},
        {8, 8}};  // TODO: Make this settable in the constructor
    std::set<std::pair<int, int>> visited = {};
    void set_border_walls();
    void reset_distances();
    void set_wall(Position pos, Direction d);
    void clear_wall(Position pos, Direction d);
    void set_distance(Position pos, int value);
    int get_distance(Position pos);
    int get_walls(Position pos);
    int maze_width();
    int maze_height();
    bool at_target(Position pos);
    bool in_bounds(Position pos);
    bool exists_wall(Position pos, Direction dir);
    bool is_dead_end(Position pos);
};
