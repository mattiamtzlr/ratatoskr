#pragma once
#include <map>
#include <queue>
#include <string>
#include <vector>

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

enum Instruction {
    MOVE_FORWARD,
    MOVE_FORWARD_HALF,
    TURN_LEFT_45,
    TURN_LEFT_90,
    TURN_RIGHT_45,
    TURN_RIGHT_90,

};

static std::map<Instruction, std::string> inst_to_str = {
    {MOVE_FORWARD, "MOVE_FORWARD"},   {MOVE_FORWARD_HALF, "MOVE_FORWARD_HALF"},
    {TURN_LEFT_45, "TURN_LEFT_45"},   {TURN_LEFT_90, "TURN_LEFT_90"},
    {TURN_RIGHT_45, "TURN_RIGHT_45"}, {TURN_RIGHT_90, "TURN_RIGHT_90"},

};

enum MODE { DEBUG, RUN, DUMP_LOG, TESTING };

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

bool operator<(const Position& lhs, const Position& rhs);

bool operator==(const Position& lhs, const Position& rhs);

struct GraphCoordinate {
    float x;
    float y;
    GraphCoordinate(float x, float y) : x(x), y(y) {}
    GraphCoordinate(Position pos) : x(pos.x), y(pos.y) {}
    GraphCoordinate() : x(0), y(0) {}
};

bool operator<(const GraphCoordinate& lhs, const GraphCoordinate& rhs);

bool operator==(const GraphCoordinate& lhs, const GraphCoordinate& rhs);

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
