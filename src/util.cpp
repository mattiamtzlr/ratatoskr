#include "util.hpp"

#include <cctype>

Direction rotate_right(Direction dir) { return (Direction)((dir + 2) % 8); }
Direction rotate_right45(Direction dir) {return (Direction)((dir + 1) % 8); }
Direction rotate_half(Direction dir) { return (Direction)((dir + 4) % 8); }
Direction rotate_left45(Direction dir) {return (Direction)((dir + 7) % 8); }
Direction rotate_left(Direction dir) { return (Direction)((dir + 6) % 8); }

int dx(Direction d) { return d == EAST ? 1 : (d == WEST ? -1 : 0); }
int dy(Direction d) { return d == NORTH ? 1 : (d == SOUTH ? -1 : 0); };

Position get_neighbor(Position pos, Direction d) {
    return Position(pos.x + dx(d), pos.y + dy(d));
}

Position get_diag_neighbor(Position pos, Direction d){
    std::map<Direction, Position> dir_to_pos = {
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

Direction dir_for_neighbor(GraphCoordinate coord_n, GraphCoordinate coord_m) {
    if (coord_n.x > coord_m.x)
        return EAST;
    else if (coord_n.x < coord_m.x)
        return WEST;

    else if (coord_n.y > coord_m.y)
        return NORTH;

    else
        return SOUTH;
}
