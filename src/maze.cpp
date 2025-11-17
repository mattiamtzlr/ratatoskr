#include "maze.hpp"
#include <map>

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

bool Maze::can_move_diag(Position pos, DiagDirection dir){
    bool path1_clear = false;
    bool path2_clear = false;
    Position target_diag = Position(0,0);

    switch (dir) {
        case NORTH_EAST:
            target_diag = get_diag_neighbor(pos, NORTH_EAST);
            if (!in_bounds(target_diag)) return false;

            path1_clear = !exists_wall(pos, NORTH) && 
                          !exists_wall(get_neighbor(pos, NORTH), EAST);
            
            path2_clear = !exists_wall(pos, EAST) &&
                          !exists_wall(get_neighbor(pos, EAST), NORTH);
            break;

        case SOUTH_EAST:
            target_diag = get_diag_neighbor(pos, SOUTH_EAST);
            if (!in_bounds(target_diag)) return false;

            path1_clear = !exists_wall(pos, SOUTH) &&
                          !exists_wall(get_neighbor(pos, SOUTH), EAST);

            path2_clear = !exists_wall(pos, EAST) &&
                          !exists_wall(get_neighbor(pos, EAST), SOUTH);
            break;

        case SOUTH_WEST:
            target_diag = get_diag_neighbor(pos, SOUTH_WEST);
            if (!in_bounds(target_diag)) return false;

            path1_clear = !exists_wall(pos, SOUTH) &&
                          !exists_wall(get_neighbor(pos, SOUTH), WEST);

            path2_clear = !exists_wall(pos, WEST) &&
                          !exists_wall(get_neighbor(pos, WEST), SOUTH);
            break;

        case NORTH_WEST:
            target_diag = get_diag_neighbor(pos, NORTH_WEST);
            if (!in_bounds(target_diag)) return false;

            path1_clear = !exists_wall(pos, NORTH) &&
                          !exists_wall(get_neighbor(pos, NORTH), WEST);

            path2_clear = !exists_wall(pos, WEST) &&
                          !exists_wall(get_neighbor(pos, WEST), NORTH);
            break;
    }

    return path1_clear || path2_clear;
}

std::vector<Position> Maze::valid_diag_neighbors(Position mouse_pos) {
    std::vector<Position> neigh;
    for (DiagDirection k : {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST}) {
        if (can_move_diag(mouse_pos, k)) {
            neigh.push_back(get_diag_neighbor(mouse_pos, k));
        }
    }
    return neigh;
}

std::vector<std::vector<Position>> Maze::find_diagonal_paths(int min_length) {
    std::vector<std::vector<Position>> all_paths;
    
    bool diag_path_visited[MAZE_WIDTH][MAZE_HEIGHT] = {};

    for (const Position& pos : visited) {
        
        if (diag_path_visited[pos.x][pos.y]) {
            continue;
        }

        for (DiagDirection d_dir : {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST}) {
            std::vector<Position> current_path;
            Position current_pos = pos;

            while (
                in_bounds(current_pos) &&           
                visited.count(current_pos) &&       
                !diag_path_visited[current_pos.x][current_pos.y]
            ) {
                current_path.push_back(current_pos);

                if (!can_move_diag(current_pos, d_dir)) {
                    break;
                }

                current_pos = get_diag_neighbor(current_pos, d_dir);
            }

            if (current_path.size() >= min_length + 1) {
                all_paths.push_back(current_path);
                
                for (const Position& p : current_path) {
                    diag_path_visited[p.x][p.y] = true;
                }
            }
        }
    }
    return all_paths;
}