#include "maze.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

const int MOVE_COST = 2;
const int HALF_MOVE_COST = 1;
const int TURN_COST = 10;

Maze::Maze() { set_border_walls(); }

std::vector<Position> Maze::valid_neighbors(Position mouse_pos) {
    std::vector<Position> neigh;
    for (Direction k : {NORTH, EAST, SOUTH, WEST}) {
        Position neighbor = get_neighbor(mouse_pos, k);
        if (in_bounds(neighbor) && !exists_wall(mouse_pos, k)) {
            neigh.push_back(neighbor);
        }
    }
    return neigh;
}

void Maze::set_border_walls() {
    for (int x = 0; x < MAZE_WIDTH; ++x) {
        set_wall(Position(x, 0), SOUTH);
        set_wall(Position(x, MAZE_HEIGHT - 1), NORTH);
    }
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        set_wall(Position(0, y), WEST);
        set_wall(Position(MAZE_WIDTH - 1, y), EAST);
    }
}
void Maze::set_wall(Position pos, Direction d) {
    m_wall_storage[pos.x][pos.y] |= (0b1 << d);
    Position front_neighbor = get_neighbor(pos, d);
    if (in_bounds(front_neighbor))
        m_wall_storage[front_neighbor.x][front_neighbor.y] |=
            (0b1 << rotate_half(d));
}

void Maze::clear_wall(Position pos, Direction d) {
    m_wall_storage[pos.x][pos.y] &= (~(0b1 << d));
}

int Maze::get_walls(Position pos) { return m_wall_storage[pos.x][pos.y]; }

void Maze::reset_distances() {
    for (int x = 0; x < MAZE_WIDTH; ++x)
        for (int y = 0; y < MAZE_HEIGHT; ++y)
            m_distances[x][y] = MAZE_WIDTH * MAZE_HEIGHT + 1;
}

int Maze::maze_height() { return MAZE_HEIGHT; }

int Maze::maze_width() { return MAZE_WIDTH; }

bool Maze::at_target(Position pos) {
    for (int i = 0; i < targets.size(); i++) {
        if (targets[i].x == pos.x && targets[i].y == pos.y) {
            return true;
        }
    }
    return false;
}
void Maze::set_distance(Position pos, int value) {
    m_distances[pos.x][pos.y] = value;
}
int Maze::get_distance(Position pos) { return m_distances[pos.x][pos.y]; }

bool Maze::in_bounds(Position pos) {
    return 0 <= pos.x && pos.x < MAZE_WIDTH && 0 <= pos.y &&
           pos.y < MAZE_HEIGHT;
}

bool Maze::exists_wall(Position pos, Direction dir) {
    // the following should turn non-zero to true and zero to false.
    return (bool)(get_walls(pos) & 0b1 << dir);
}

bool Maze::can_move_diag(Position pos, Direction dir) {
    Position target_diag = get_diag_neighbor(pos, dir);
    if (!in_bounds(target_diag)) return false;
    return !exists_wall(pos, diagDirFirst[dir]) &&
               !exists_wall(get_neighbor(pos, diagDirFirst[dir]),
                            diagDirSecond[dir]) ||
           !exists_wall(pos, diagDirSecond[dir]) &&
               !exists_wall(get_neighbor(pos, diagDirSecond[dir]),
                            diagDirFirst[dir])

        ;
}

std::map<GraphCoordinate, std::set<Edge>>& Maze::get_adj_list(
    std::map<GraphCoordinate, std::set<Edge>>& adj_list) {
    std::set<GraphCoordinate>* halfway_nodes = new std::set<GraphCoordinate>;

    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            GraphCoordinate p = GraphCoordinate(x, y);
            // ensure the node appears in the adjacency list even if it has
            // no neighbors
            for (Position n : valid_neighbors(Position(p.x, p.y))) {
                GraphCoordinate neighbor = GraphCoordinate(n);
                GraphCoordinate inbetween =
                    GraphCoordinate((n.x + p.x) / 2, (n.y + p.y) / 2);

                Edge e = {neighbor, MOVE_COST};
                Edge e_back = {p, MOVE_COST};
                Edge e_half = {inbetween, HALF_MOVE_COST};
                Edge e_back_half = {p, HALF_MOVE_COST};

                adj_list[inbetween].insert(e_back_half);
                adj_list[p].insert(e_half);
                adj_list[p].insert(e);
                adj_list[neighbor].insert(e_back);
                (*halfway_nodes).insert(inbetween);
            }
        }
    }

    for (const GraphCoordinate& node : (*halfway_nodes)) {
        for (Direction d : {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST}) {
            GraphCoordinate diag_neighbor = get_diag_neighbor(node, d);
            if (std::find((*halfway_nodes).begin(), (*halfway_nodes).end(),
                          diag_neighbor) != (*halfway_nodes).end()) {
                Edge e = {diag_neighbor, HALF_MOVE_COST};
                Edge e_back = {node, HALF_MOVE_COST};

                adj_list[node].insert(e);
                adj_list[diag_neighbor].insert(e_back);
            }
        }
    }

    return adj_list;
}
bool Maze::in_visited(Position pos) {
    return std::find(visited.begin(), visited.end(), pos) != visited.end();
}
float Maze::distance_to_target_L2(Position pos) {
    float min_distance = .0;
    for (Position target : targets) {
        float distance = std::sqrt(std::pow((pos.x - target.x), 2) +
                                   std::pow((pos.y - target.y), 2));
        if (min_distance < distance) {
            min_distance = distance;
        }
    }
    return min_distance;
}
