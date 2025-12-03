#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "config.hpp"
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
    std::vector<Position> m_targets;
    void clear_wall(Position pos, Direction d);
    int get_walls(Position pos);
    bool in_bounds(Position pos);
    void set_border_walls();
    bool can_move_diag(Position pos, Direction dir);

   public:
    std::set<Position> visited = {};
    Maze();
    void set_targets(const std::vector<Position>& targets);
    const std::vector<Position>& get_targets() const;
    std::vector<Position> valid_neighbors(Position mouse_pos);
    void reset_distances();
    void set_wall(Position pos, Direction d);
    void set_distance(Position pos, int value);
    int get_distance(Position pos);
    float distance_to_target_L2(Position pos);
    bool at_target(Position pos);
    bool exists_wall(Position pos, Direction dir);
    void finalize_discovery();
    std::map<GraphCoordinate, std::set<Edge>>& get_adj_list(
        std::map<GraphCoordinate, std::set<Edge>>& adj_list);
    bool in_visited(Position pos);
};
