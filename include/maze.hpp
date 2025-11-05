#pragma once

#include <cstddef>
#include "util.hpp"

constexpr int MAZE_WIDTH  = 8;
constexpr int MAZE_HEIGHT = 8;

class Maze {
  private:
    int m_wall_storage[MAZE_HEIGHT][MAZE_WIDTH] = {};
    int m_distances[MAZE_HEIGHT][MAZE_WIDTH]    = {};

  public:
    void set_wall(size_t x, size_t y, Direction d);
    void clear_wall(size_t x, size_t y, Direction d);
    int get_walls_at(size_t x, size_t y);
    int maze_width();
    int maze_height();
};
