#include "maze.hpp"

void Maze::set_wall(size_t x, size_t y, Direction d) {
    m_wall_storage[x][y] |= (0b1 << d);
}

void Maze::clear_wall(size_t x, size_t y, Direction d) {
    m_wall_storage[x][y] &= (~(0b1 << d));
}

int Maze::get_walls_at(size_t x, size_t y) { return m_wall_storage[x][y]; }

void Maze::reset_distances() {
    for (int x = 0; x < MAZE_WIDTH; ++x)
        for (int y = 0; y < MAZE_HEIGHT; ++y)
            m_distances[x][y] = MAZE_WIDTH * MAZE_HEIGHT;
}
int Maze::maze_height() { return MAZE_HEIGHT; }

int Maze::maze_width() { return MAZE_WIDTH; }

bool Maze::at_target(Position pos) {
    for (int i = 0; i < 4; i++) {
        if (targets[i].x == pos.x && targets[i].y == pos.y) {
            return true;
        }
    }
    return false;
}
bool Maze::in_bounds(Position pos) {
    return 0 <= pos.x && pos.x < MAZE_WIDTH && 0 <= pos.y &&
           pos.y < MAZE_HEIGHT;
}
