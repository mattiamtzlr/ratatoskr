#include "util.hpp"

Direction rotate_right(Direction dir) { return (Direction)((dir + 2) % 8); }
Direction rotate_right45(Direction dir) { return (Direction)((dir + 1) % 8); }
Direction rotate_half(Direction dir) { return (Direction)((dir + 4) % 8); }
Direction rotate_left45(Direction dir) { return (Direction)((dir + 7) % 8); }
Direction rotate_left(Direction dir) { return (Direction)((dir + 6) % 8); }

int dx(Direction d) { return d == EAST ? 1 : (d == WEST ? -1 : 0); }
int dy(Direction d) { return d == NORTH ? 1 : (d == SOUTH ? -1 : 0); };

Position get_neighbor(Position pos, Direction d) {
    return Position(pos.x + dx(d), pos.y + dy(d));
}

Position get_diag_neighbor(Position pos, Direction d) {
    switch (d) {
        case NORTH_EAST:
            return Position(pos.x + 1, pos.y + 1);
        case SOUTH_EAST:
            return Position(pos.x + 1, pos.y - 1);
        case NORTH_WEST:
            return Position(pos.x - 1, pos.y + 1);
        case SOUTH_WEST:
            return Position(pos.x - 1, pos.y - 1);
        default:
            return Position();
    };
}
GraphCoordinate get_diag_neighbor(GraphCoordinate pos, Direction d) {
    switch (d) {
        case NORTH_EAST:
            return GraphCoordinate(pos.x + 0.5, pos.y + 0.5);
        case SOUTH_EAST:
            return GraphCoordinate(pos.x + 0.5, pos.y - 0.5);
        case NORTH_WEST:
            return GraphCoordinate(pos.x - 0.5, pos.y + 0.5);
        case SOUTH_WEST:
            return GraphCoordinate(pos.x - 0.5, pos.y - 0.5);
        default:
            return GraphCoordinate();
    };
}

bool isInteger(double n) {
    int x = n;
    return !((double)(n - x) > 0);
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
    if (coord_n.x > coord_m.x && coord_n.y == coord_m.y)
        return EAST;
    else if (coord_n.x < coord_m.x && coord_n.y == coord_m.y)
        return WEST;
    else if (coord_n.x == coord_m.x && coord_n.y > coord_m.y)
        return NORTH;
    else if (coord_n.x == coord_m.x && coord_n.y < coord_m.y)
        return SOUTH;
    else if (coord_n.x > coord_m.x && coord_n.y > coord_m.y)
        return NORTH_EAST;
    else if (coord_n.x > coord_m.x && coord_n.y < coord_m.y)
        return SOUTH_EAST;
    else if (coord_n.x < coord_m.x && coord_n.y < coord_m.y)
        return SOUTH_WEST;
    else if (coord_n.x < coord_m.x && coord_n.y > coord_m.y)
        return NORTH_WEST;
    else
        return NORTH;
}

bool operator<(const GraphCoordinate& lhs, const GraphCoordinate& rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

bool operator==(const GraphCoordinate& lhs, const GraphCoordinate& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator<(const Position& lhs, const Position& rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

double L2_distance_squared(GraphCoordinate c_1, GraphCoordinate c_2) {
    return (c_1.x - c_2.x) * (c_1.x - c_2.x) +
           (c_1.y - c_2.y) * (c_1.y - c_2.y);
}
