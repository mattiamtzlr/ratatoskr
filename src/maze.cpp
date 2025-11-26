#include "maze.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>

const int MOVE_COST = 1;
const int TURN_COST = 5;

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

bool Maze::is_dead_end(Position pos) {
    for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
        if (!exists_wall(pos, d) &&
            (get_distance(get_neighbor(pos, d)) < get_distance(pos))) {
            return false;
        }
    }
    return true;
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

std::vector<Position> Maze::valid_diag_neighbors(Position mouse_pos) {
    std::vector<Position> neigh;
    for (Direction k : {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST}) {
        if (can_move_diag(mouse_pos, k)) {
            neigh.push_back(get_diag_neighbor(mouse_pos, k));
        }
    }
    return neigh;
}

std::vector<std::vector<Position>> Maze::find_diagonal_paths(
    int min_length, std::vector<Position> path) {
    std::vector<std::vector<Position>> all_paths;

    bool diag_path_visited[MAZE_WIDTH][MAZE_HEIGHT] = {};

    for (Position pos : path) {
        if (diag_path_visited[pos.x][pos.y]) {
            continue;
        }
        for (Position neighbor : valid_neighbors(pos)) {
            Position starting_neighbor = neighbor;
            if (std::find(path.begin(), path.end(), neighbor) == path.end())
                continue;
            for (int i = 0; i < 2; i++) {  // Right diag and left diag
                std::vector<Position> current_path;
                current_path.push_back(pos);
                current_path.push_back(neighbor);
                bool right = i == 0;
                Direction dir =
                    (right) ? rotate_right(dir_for_neighbor(neighbor, pos))
                            : rotate_left(dir_for_neighbor(neighbor, pos));

                while (!exists_wall(neighbor, dir) &&
                       std::find(path.begin(), path.end(), neighbor) !=
                           path.end()) {
                    neighbor = get_neighbor(neighbor, dir);
                    right = !right;
                    dir = (right) ? rotate_right(dir) : rotate_left(dir);
                    diag_path_visited[neighbor.x][neighbor.y] = true;
                    current_path.push_back(neighbor);
                }
                if (current_path.size() > min_length * 2) {
                    all_paths.push_back(current_path);
                    diag_path_visited[starting_neighbor.x]
                                     [starting_neighbor.y] = true;
                }
            }
        }
    }
    return all_paths;
}

std::map<Position, std::vector<Edge>> Maze::get_adj_list() {
    std::map<Position, std::vector<Edge>> adj_list;

    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            Position p = Position(x, y);
            // ensure the node appears in the adjacency list even if it has
            // no neighbors
            for (Position neighbor : valid_neighbors(p)) {
                Edge e = {neighbor, MOVE_COST};
                Edge e_back = {p, MOVE_COST};
                adj_list[p].push_back(e);
                adj_list[neighbor].push_back(e_back);
            }
        }
    }
    /*
    for (std::vector<Position> diagonal : find_diagonal_paths()) {
        for (int i = 0; i < diagonal.size() - 1; i++) {
            Position pos_1 = diagonal[i];
            Position pos_2 = diagonal[i + 1];
            Edge edge_1_to_2 = {pos_2,
                                MOVE_COST};  // TODO: bit harder to
    calculate
                                             // cost of turns for diagonals
            Edge edge_2_to_1 = {pos_1, MOVE_COST};
            adj_list[pos_1].push_back(edge_1_to_2);
            adj_list[pos_2].push_back(edge_2_to_1);
        }
    }
    */

    return adj_list;
}
bool Maze::in_visited(Position pos) {
    for (Position checked : visited) {
        if (checked.x == pos.x && checked.y == pos.y) {
            return true;
        }
    }
    return false;
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
