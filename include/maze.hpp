#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "config.hpp"
#include "util.hpp"

constexpr int MOVE_COST = 10;
constexpr int HALF_MOVE_COST = 5;
constexpr int DIAG_HALF_MOVE_COST = 7;

struct Edge {
    GraphCoordinate target;
    unsigned int weight;
};

inline bool operator<(const Edge& lhs, const Edge& rhs) {
    if (lhs.target.x != rhs.target.x) return lhs.target.x < rhs.target.x;
    if (lhs.target.y != rhs.target.y) return lhs.target.y < rhs.target.y;
    return lhs.weight < rhs.weight;
}

class Maze {
   private:
    uint8_t m_wall_storage[Config::MAZE_WIDTH][Config::MAZE_HEIGHT] = {};
    int m_distances[Config::MAZE_WIDTH][Config::MAZE_HEIGHT] = {};
    std::vector<Position> m_targets;
    void clear_wall(Position pos, Direction d);
    int get_walls(Position pos);
    bool in_bounds(Position pos);
    void set_border_walls();
    bool can_move_diag(Position pos, Direction dir);

   public:
    std::set<Position> visited = {};

    /**
     * Constructor for the maze.
     * Will initialise border walls according to config.
     */
    Maze();

    /**
     * Set the targets for the current maze.
     * Takes vector of targets
     * Need to be in bounds.
     */
    void set_targets(const std::vector<Position>& targets);

    /**
     * Getter for the targets vector.
     */
    const std::vector<Position>& get_targets() const;

    /**
     * Calculate valid neighboring cells. A valid neighbor is a neighbor that
     * can be reached without passing through a wall.
     * returns a vector of valid neighbors.
     */
    std::vector<Position> valid_neighbors(Position mouse_pos);

    /**
     * Set all distances to `WIDTH · HEIGHT + 1`
     */
    void reset_distances();

    /**
     * Set wall at position in direction
     * Also sets the wall for the cell on the other side.
     */
    void set_wall(Position pos, Direction d);

    /**
     * Set the distance at a position to the value.
     */
    void set_distance(Position pos, int value);

    /**
     * Getter for the distance at a position.
     */
    int get_distance(Position pos);

    /**
     * Returns Euclidian distance to closest target cell.
     */
    float distance_to_target_L2(Position pos);

    /**
     * Returns true if at target false o/w
     */
    bool at_target(Position pos);

    /**
     * Takes a position and a direction
     * Returns true if at position in direction there is a wall.
     */
    bool exists_wall(Position pos, Direction dir);

    /**
     * Calculate the adjacency list of (half-)cells in the discovered maze.
     * If diagonals is set to false it will not include diagonal edges.
     */
    std::map<GraphCoordinate, std::set<Edge>>& get_adj_list(
        std::map<GraphCoordinate, std::set<Edge>>& adj_list,
        bool diagonals = true);

    /**
     * Return true if position was visited.
     */
    bool in_visited(Position pos);
};
