#pragma once
#include <map>
#include <queue>
#include <vector>

constexpr int CELL_SIZE_MM = 180;     // 18 cm
constexpr int WALL_TO_WALL_MM = 160;  // 16 cm

enum Direction {
    NORTH = 0,
    NORTH_EAST = 1,
    EAST = 2,
    SOUTH_EAST = 3,
    SOUTH = 4,
    SOUTH_WEST = 5,
    WEST = 6,
    NORTH_WEST = 7,
};

enum MODE { RUN, DUMP_LOG, TESTING };

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
    Position(size_t x, size_t y) : x(x), y(y) {};
    Position() : x(0), y(0) {};
};

inline bool operator<(const Position& lhs, const Position& rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

inline bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

struct GraphCoordinate {
    float x;
    float y;
    GraphCoordinate(float x, float y) : x(x), y(y) {}
    GraphCoordinate(Position pos) : x(pos.x), y(pos.y) {}
    GraphCoordinate() : x(0), y(0) {}
};

inline bool operator<(const GraphCoordinate& lhs, const GraphCoordinate& rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

inline bool operator==(const GraphCoordinate& lhs, const GraphCoordinate& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
struct PQCmp {
    bool operator()(const std::pair<int, GraphCoordinate>& a,
                    const std::pair<int, GraphCoordinate>& b) const {
        return a.first > b.first;
    }
};

typedef std::priority_queue<std::pair<int, GraphCoordinate>,
                            std::vector<std::pair<int, GraphCoordinate>>, PQCmp>
    p_queue;

Direction rotate_right(Direction dir);
Direction rotate_right45(Direction dir);
Direction rotate_left(Direction dir);
Direction rotate_left45(Direction dir);

Direction rotate_half(Direction dir);
Direction dir_for_neighbor(Position pos_n, Position pos_m);
Direction dir_for_neighbor(GraphCoordinate pos_n, GraphCoordinate pos_m);

bool isInteger(double n);
int dx(Direction d);
int dy(Direction d);

Position get_neighbor(Position pos, Direction d);
Position get_diag_neighbor(Position pos, Direction d);
GraphCoordinate get_diag_neighbor(GraphCoordinate pos, Direction d);
double L2_distance_squared(GraphCoordinate c_1, GraphCoordinate c_2);
