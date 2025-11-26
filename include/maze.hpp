#pragma once

#include <array>
#include <cstddef>
#include <set>
#include <vector>

#include "util.hpp"

constexpr int MAZE_WIDTH = 16;
constexpr int MAZE_HEIGHT = 16;

struct Edge {
    Position target;
    unsigned int weight;
};

class Maze {
   private:
    int m_wall_storage[MAZE_HEIGHT][MAZE_WIDTH] = {};
    int m_distances[MAZE_HEIGHT][MAZE_WIDTH] = {};

   public:
    std::vector<Position> targets;
    std::set<Position> visited = {};
    std::set<Position> diags = {};
    std::set<Position> turns = {};
    std::vector<Position> valid_neighbors(Position mouse_pos);
    void set_border_walls();
    void reset_distances();
    void set_wall(Position pos, Direction d);
    void clear_wall(Position pos, Direction d);
    void set_distance(Position pos, int value);
    int get_distance(Position pos);
    int get_walls(Position pos);
    int maze_width();
    int maze_height();
    float distance_to_target_L2(Position pos);
    bool at_target(Position pos);
    bool in_bounds(Position pos);
    bool exists_wall(Position pos, Direction dir);
    bool is_dead_end(Position pos);
    bool can_move_diag(Position pos, Direction dir);
    void finalize_discovery();
    std::vector<Position> valid_diag_neighbors(Position mouse_pos);
    std::vector<std::vector<Position>> find_diagonal_paths(
        int min_length, std::vector<Position> path);
    std::map<Position, std::vector<Edge>> get_adj_list();
    bool in_visited(Position pos);  // TODO: I know that this can be than using
                                    // cpp functionality but I dont trust it
};
