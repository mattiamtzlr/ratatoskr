#include "maze.hpp"

void Maze::set_wall(Position pos, Direction d) {
    m_wall_storage[pos.x][pos.y] |= (0b1 << d);
}

void Maze::clear_wall(Position pos, Direction d) {
    m_wall_storage[pos.x][pos.y] &= (~(0b1 << d));
}

int Maze::get_walls(Position pos) { return m_wall_storage[pos.x][pos.y]; }

void Maze::reset_distances() {
    for (int x = 0; x < MAZE_WIDTH; ++x)
        for (int y = 0; y < MAZE_HEIGHT; ++y)
            m_distances[x][y] = MAZE_WIDTH * MAZE_HEIGHT + 1;
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
void Maze::set_distance(Position pos, int value) {
    m_distances[pos.x][pos.y] = value;
}
int Maze::get_distance(Position pos) { return m_distances[pos.x][pos.y]; }

bool Maze::in_bounds(Position pos) {
    return 0 <= pos.x && pos.x < MAZE_WIDTH && 0 <= pos.y &&
           pos.y < MAZE_HEIGHT;
}

bool Maze::exists_wall(Position pos, Direction dir) {
    // the following should turn non-zero to true and zero to false.
    return (bool) (get_walls(pos) & 0b1 << dir); }

bool Maze::is_dead_end(Position pos) {
    for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
        if (!exists_wall(pos, d) && (get_distance(get_neighbor(pos, d)) < get_distance(pos))) {
            return false;
        }
    }
    return true;
}
