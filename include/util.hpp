#pragma once
#include <map>

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
    NORTH_EAST = 4,
    SOUTH_EAST = 5,
    NORTH_WEST = 6,
    SOUTH_WEST = 7,
};

static std::map<Direction, char> dirToCardinalChar = {
    {NORTH, 'n'}, {EAST, 'e'}, {SOUTH, 's'}, {WEST, 'w'}};
static std::map<char, Direction> cardinalCharToDir = {
    {'n', NORTH}, {'e', EAST}, {'s', SOUTH}, {'w', WEST}};

static std::map<Direction, Direction> diagDirFirst = {{NORTH_EAST, NORTH},
                                                      {SOUTH_EAST, SOUTH},
                                                      {SOUTH_WEST, SOUTH},
                                                      {NORTH_WEST, NORTH}};
static std::map<Direction, Direction> diagDirSecond = {{NORTH_EAST, EAST},
                                                       {SOUTH_EAST, EAST},
                                                       {SOUTH_WEST, WEST},
                                                       {NORTH_WEST, WEST}};

struct Position {
    int x;
    int y;
    Position(size_t x, size_t y) : x(x), y(y) {}
};
inline bool operator<(const Position& lhs, const Position& rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}
Direction rotate_right(Direction dir);

Direction rotate_left(Direction dir);

Direction rotate_half(Direction dir);
Direction dir_for_neighbor(Position pos_n, Position pos_m);

int dx(Direction d);
int dy(Direction d);

Position get_neighbor(Position pos, Direction d);
Position get_diag_neighbor(Position pos, Direction d);
