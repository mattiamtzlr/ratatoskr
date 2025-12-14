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

/*
* Map instructions to strings
*/
static std::map<Instruction, std::string> inst_to_str = {
    {MOVE_FORWARD, "MOVE_FORWARD"},   {MOVE_FORWARD_HALF, "MOVE_FORWARD_HALF"},
    {TURN_LEFT_45, "TURN_LEFT_45"},   {TURN_LEFT_90, "TURN_LEFT_90"},
    {TURN_RIGHT_45, "TURN_RIGHT_45"}, {TURN_RIGHT_90, "TURN_RIGHT_90"},

};
 /*
 *
 * `DEBUG`: Show on oled screen useful infos (gyro, tof, etc...)
 * `RUN`: Mouse does discovery and speedrun
 * `DUMP_LOG`: Return the logs on serial monitor
 * `TESTING`: Run instructions that are on testing case in main
 */
enum MODE { DEBUG, RUN, DUMP_LOG, TESTING };

/*
* Map direction to cardinal char
*/
static std::map<Direction, char> dirToCardinalChar = {
    {NORTH, 'n'}, {EAST, 'e'}, {SOUTH, 's'}, {WEST, 'w'}};
/*
* Map cardinal char to direction
*/
static std::map<char, Direction> cardinalCharToDir = {
    {'n', NORTH}, {'e', EAST}, {'s', SOUTH}, {'w', WEST}};

/*
* Map to desired diagonal the first step to do (i.e. `NORTH_EAST` to `NORTH`)
*/
static std::map<Direction, Direction> diagDirFirst = {{NORTH_EAST, NORTH},
                                                      {SOUTH_EAST, SOUTH},
                                                      {SOUTH_WEST, SOUTH},
                                                      {NORTH_WEST, NORTH}};
/*
* Map to desired diagonal the second step to do (i.e. `NORTH_EAST` to `EAST`)
*/                                                                                                       
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

/*
* Same as `Position` but coordinates are floats (needed for the diagonals)
*/
struct GraphCoordinate {
    float x;
    float y;
    GraphCoordinate(float x, float y) : x(x), y(y) {}
    GraphCoordinate(Position pos) : x(pos.x), y(pos.y) {}
    GraphCoordinate() : x(0), y(0) {}
};

bool operator<(const GraphCoordinate& lhs, const GraphCoordinate& rhs);

bool operator==(const GraphCoordinate& lhs, const GraphCoordinate& rhs);

/*
* Priority queue comparator, needed for Dijkstra's algorithm
*/
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
/*
* Get the direction the mouse has to face to go from `pos_m` to `pos_n`
*/
Direction dir_for_neighbor(Position pos_n, Position pos_m);
/*
* Get the direction the mouse has to face to go from `pos_m` to `pos_n`
*/
Direction dir_for_neighbor(GraphCoordinate pos_n, GraphCoordinate pos_m);

bool isInteger(double n);
/*
* Get the delta x fro a given direction
*/
int dx(Direction d);
/*
* Get the delta y fro a given direction
*/
int dy(Direction d);

/*
* Get the neighbor position in direction d
*/
Position get_neighbor(Position pos, Direction d);
/*
* Get the diagonal neighbor position in direction d
*/
Position get_diag_neighbor(Position pos, Direction d);
/*
* Get the diagonal neighbor graph coordinate in direction d
*/
GraphCoordinate get_diag_neighbor(GraphCoordinate pos, Direction d);
/*
* Compute the ecuclidean distance squared between two coordinates
*/
double L2_distance_squared(GraphCoordinate c_1, GraphCoordinate c_2);
