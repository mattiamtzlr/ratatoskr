#pragma once
#include <map>

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
};

std::map<Direction, char> dirToCardinalChar = {
    {NORTH, 'n'}, {EAST, 'e'}, {SOUTH, 's'}, {WEST, 'w'}};
std::map<char, Direction> cardinalCharToDir = {
    {'n', NORTH}, {'e', EAST}, {'s', SOUTH}, {'w', WEST}};

struct Position {
    size_t x;
    size_t y;
    Position(size_t x, size_t y) : x(x), y(y) {}
};

Direction rotate_right(Direction dir);

Direction rotate_left(Direction dir);
