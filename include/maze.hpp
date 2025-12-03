#pragma once

#include "config.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "util.hpp"

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
    uint8_t m_wall_storage[Config::MAZE_HEIGHT][Config::MAZE_WIDTH] = {};
    int m_distances[Config::MAZE_HEIGHT][Config::MAZE_WIDTH] = {};
    void clear_wall(Position pos, Direction d);
    int get_walls(Position pos);
    bool in_bounds(Position pos);
    void set_border_walls();
    bool can_move_diag(Position pos, Direction dir);

   public:
    std::vector<Position> targets;
    std::set<Position> visited = {};
    Maze();
    std::vector<Position> valid_neighbors(Position mouse_pos);
    void reset_distances();
    void set_wall(Position pos, Direction d);
    void set_distance(Position pos, int value);
    int get_distance(Position pos);
    int maze_width();
    int maze_height();
    float distance_to_target_L2(Position pos);
    bool at_target(Position pos);
    bool exists_wall(Position pos, Direction dir);
    void finalize_discovery();
    std::map<GraphCoordinate, std::set<Edge>>& get_adj_list(
        std::map<GraphCoordinate, std::set<Edge>>& adj_list);
    bool in_visited(Position pos);
};
