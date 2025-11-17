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

Position get_diag_neighbor(Position pos, DiagDirection d){
    std::map<DiagDirection, Position> dir_to_pos = {
        {NORTH_EAST, Position(pos.x + 1, pos.y + 1)},
        {SOUTH_EAST, Position(pos.x + 1, pos.y - 1)},
        {NORTH_WEST, Position(pos.x - 1, pos.y + 1)},
        {SOUTH_WEST, Position(pos.x - 1, pos.y - 1)},
    };

    return dir_to_pos.at(d);
}

Direction dir_for_neighbor(Position pos_n, Position pos_m) {
    if (pos_n.x > pos_m.x)
        return EAST;
    else if (pos_n.x < pos_m.x)
        return WEST;

    else if (pos_n.y > pos_m.y)
        return NORTH;

    else
        return SOUTH;
}
