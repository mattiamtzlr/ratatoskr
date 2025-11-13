#pragma once

#include <array>
#include <cstddef>
#include <set>
#include <vector>

#include "util.hpp"

constexpr int MAZE_WIDTH = 16;
constexpr int MAZE_HEIGHT = 16;

class Maze {
   private:
    int m_wall_storage[MAZE_HEIGHT][MAZE_WIDTH] = {};
    int m_distances[MAZE_HEIGHT][MAZE_WIDTH] = {};

   public:
    std::vector<Position> targets;
    std::set<Position> visited = {};
    std::vector<Position> valid_neighbors(Position mouse_pos);
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
