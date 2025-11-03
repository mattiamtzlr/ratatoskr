#include "maze.hpp"

void Maze::set_wall(size_t x, size_t y, Direction d) {
  m_wall_storage[x][y] |= d;
}

void Maze::clear_wall(size_t x, size_t y, Direction d) {
  m_wall_storage[x][y] &= (~d);
}

int Maze::get_walls_at(size_t x, size_t y) {
    return m_wall_storage[x][y];
}

int Maze::maze_height() {
    return MAZE_HEIGHT;
}

int Maze::maze_width() {
    return MAZE_WIDTH;
}
