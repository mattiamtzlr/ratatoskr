#pragma once

#include <cstddef>

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
    void reset_distances();
    void set_wall(size_t x, size_t y, Direction d);
    void clear_wall(size_t x, size_t y, Direction d);
    void set_distance(size_t x, size_t y, int value);
    int get_distance(size_t x, size_t y);
    int get_walls(size_t x, size_t y);
    int maze_width();
    int maze_height();
    bool at_target(Position pos);
    bool in_bounds(Position pos);
    bool exists_wall(Position pos, Direction dir);
};
