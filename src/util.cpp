#include "util.hpp"

#include <cctype>

Direction rotate_right(Direction dir) { return (Direction)((dir + 1) % 4); }
Direction rotate_half(Direction dir) { return (Direction)((dir + 2) % 4); }
Direction rotate_left(Direction dir) { return (Direction)((dir + 3) % 4); }

int dx(Direction d) { return d == EAST ? 1 : (d == WEST ? -1 : 0); }
int dy(Direction d) { return d == NORTH ? 1 : (d == SOUTH ? -1 : 0); };

Position get_neighbor(Position pos, Direction d) {
    return Position(pos.x + dx(d), pos.y + dy(d));
}
