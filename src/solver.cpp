#include "solver.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <map>
#include <queue>
#include <tuple>
#include "config.hpp"

using namespace Config;

Solver::Solver(Mouse& mouse, Maze& maze) : m_mouse(mouse), m_maze(maze) {
    width = MAZE_WIDTH;
    height = MAZE_HEIGHT;
    UBOUND_DIST = height * width + 1;
}

void Solver::face(Direction target_dir) {
    /* TODO: The control flow needs some comments */
    int diff = (target_dir - m_mouse.getDirection() + 8) % 8;
    int i;
    if (diff == 4) {
        m_mouse.turn180();
    } else if (diff >= 5) {
        for (i = 0; i < 7 - diff; i += 2) m_mouse.turnLeft();
        if (i < 8 - diff) m_mouse.turnLeft45();
    } else {
        for (i = 0; i < diff - 1; i += 2) m_mouse.turnRight();
        if (i < diff) m_mouse.turnRight45();
    }
}

// detect_walls & log
void Solver::detect_and_set_walls() {
    if (m_mouse.wallFront())
        m_maze.set_wall(m_mouse.getPosition(), m_mouse.getDirection());
    if (m_mouse.wallLeft())
        m_maze.set_wall(m_mouse.getPosition(),
                        rotate_left(m_mouse.getDirection()));
    if (m_mouse.wallRight())
        m_maze.set_wall(m_mouse.getPosition(),
                        rotate_right(m_mouse.getDirection()));
}

// Flood-fill bfs_recompute
void Solver::bfs() {
    m_maze.reset_distances();
    std::deque<Position>* q = new std::deque<Position>;

    for (Position& g : m_maze.targets) {
        m_maze.set_distance(g, 0);
        q->push_back(g);
    }

    while (!q->empty()) {
        Position pos = q->front();
        q->pop_front();
        for (Position neighbor : m_maze.valid_neighbors(pos)) {
            int new_distance = m_maze.get_distance(pos) + 1;
            if (new_distance < m_maze.get_distance(neighbor)) {
                m_maze.set_distance(neighbor, new_distance);
                q->emplace_back(neighbor);
            }
        }
    }
    delete q;
};

void Solver::solve() {
    m_maze.visited.insert(m_mouse.getPosition());

    while (!m_maze.at_target(m_mouse.getPosition())) {
        detect_and_set_walls();
        bfs();
        m_mouse.update_visuals(m_maze);
        Direction best_dir = NORTH;
        int best_val = UBOUND_DIST;
        float best_dist = 0;
        for (Position neighbor :
             m_maze.valid_neighbors(m_mouse.getPosition())) {
            if (m_maze.get_distance(neighbor) < best_val ||
                (m_maze.get_distance(neighbor) == best_val &&
                 m_maze.distance_to_target_L2(neighbor) < best_dist)) {
                best_val = m_maze.get_distance(neighbor);
                best_dir = dir_for_neighbor(neighbor, m_mouse.getPosition());
                best_dist = m_maze.distance_to_target_L2(neighbor);
            }
        }
        // Identify turns to use them later for finding best path in the
        // speedrun
        Direction curr_dir = m_mouse.getDirection();
        Position pos_before = m_mouse.getPosition();
        face(best_dir);
        m_mouse.moveForward();
        m_maze.visited.insert(m_mouse.getPosition());
    }
}

std::vector<GraphCoordinate>& Solver::dijkstra(
    std::vector<GraphCoordinate>& path) {
    std::map<GraphCoordinate, std::set<Edge>>* adj_list =
        new std::map<GraphCoordinate, std::set<Edge>>;
    m_maze.get_adj_list(*adj_list);

    // prio queue of (distance, position) with comparator on distance only
    p_queue pq;
    const GraphCoordinate start = GraphCoordinate(m_mouse.getPosition());

    // distance and predecessor maps
    std::map<GraphCoordinate, int>* dist = new std::map<GraphCoordinate, int>;
    std::map<GraphCoordinate, GraphCoordinate>* prev =
        new std::map<GraphCoordinate, GraphCoordinate>;

    // initialize distances for all vertices (keys) and for all edge targets
    for (std::pair<GraphCoordinate, std::set<Edge>> kv : *adj_list)
        (*dist)[kv.first] = UBOUND_DIST;

    (*dist)[start] = 0;
    pq.push(std::make_pair(0, start));

    GraphCoordinate found_target = start;
    bool found = false;
    while (!pq.empty()) {
        std::pair<int, GraphCoordinate> top = pq.top();
        int d = top.first;
        GraphCoordinate u = top.second;
        pq.pop();

        if (isInteger(u.x) && isInteger(u.y) &&
            m_maze.at_target(Position(u.x, u.y))) {
            found_target = u;
            found = true;
            break;
        }

        for (const Edge& e : (*adj_list)[u]) {
            int alt = (*dist)[u] + e.weight;
            if (alt < (*dist)[e.target]) {
                (*dist)[e.target] = alt;
                // update predecessor without requiring default-constructible
                // GraphCoordinate
                prev->erase(e.target);
                prev->insert(std::make_pair(e.target, u));
                pq.push(std::make_pair(alt, e.target));
            }
        }
    }
    delete dist;
    delete adj_list;

    // reconstruct path from start -> found_target
    GraphCoordinate cur = found_target;
    while (!(cur.x == start.x && cur.y == start.y)) {
        path.push_back(cur);
        cur = (*prev)[cur];
    }
    delete prev;

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

void Solver::finalize_discovery() {
    /* Assume all unchecked wall positions to have walls. */
    for (int x = 0; x < MAZE_WIDTH; ++x) {
        for (int y = 0; y < MAZE_HEIGHT; ++y) {
            const Position pos = Position(x, y);
            if (!m_maze.in_visited(pos)) {
                for (const Position n : m_maze.valid_neighbors(pos)) {
                    if (!m_maze.in_visited(n))
                        m_maze.set_wall(n, dir_for_neighbor(pos, n));
                }
            }
        }
    }
    m_mouse.update_visuals(m_maze);
}

bool in_diags(const std::vector<std::vector<Position>>& diagonals,
              Position pos) {
    for (const std::vector<Position>& diagonal : diagonals) {
        if (std::find(diagonal.begin(), diagonal.end(), pos) != diagonal.end())
            return true;
    }
    return false;
}
std::vector<Position> get_diag(
    const std::vector<std::vector<Position>>& diagonals, Position pos) {
    for (const std::vector<Position>& diagonal : diagonals) {
        if (std::find(diagonal.begin(), diagonal.end(), pos) != diagonal.end())
            return diagonal;
    }
    return {};
}

void Solver::filter_turns(std::vector<Instruction>& instr) {
    for (int i = 0; i < instr.size() - 2; i++) {
        if (instr[i] == TURN_LEFT_45 && instr[i + 1] == MOVE_FORWARD_HALF &&
            instr[i + 2] == TURN_LEFT_45) {
            instr[i] = MOVE_FORWARD_HALF;
            instr[i + 1] = TURN_LEFT_90;
            instr[i + 2] = MOVE_FORWARD_HALF;

        } else if (instr[i] == TURN_RIGHT_45 &&
                   instr[i + 1] == MOVE_FORWARD_HALF &&
                   instr[i + 2] == TURN_RIGHT_45) {
            instr[i] = MOVE_FORWARD_HALF;
            instr[i + 1] = TURN_RIGHT_90;
            instr[i + 2] = MOVE_FORWARD_HALF;
        }
    }
}
std::vector<Instruction>& Solver::parse_path(
    std::vector<GraphCoordinate>& path,
    std::vector<Instruction>& instructions) {
    GraphCoordinate mouse_coordinate;
    Direction mouse_direction = NORTH;
    for (GraphCoordinate next_coordinate : path) {
        if (next_coordinate.x == mouse_coordinate.x &&
            next_coordinate.y == mouse_coordinate.y)
            continue;
        Direction next_dir =
            dir_for_neighbor(next_coordinate, mouse_coordinate);

        int diff = (next_dir - mouse_direction + 8) % 8;
        if (diff >= 5) {
            int i;
            for (i = 0; i < 7 - diff; i += 2)
                instructions.push_back(TURN_LEFT_90);
            if (i < 8 - diff) instructions.push_back(TURN_LEFT_45);
        } else {
            int i;
            for (i = 0; i < diff - 1; i += 2)
                instructions.push_back(TURN_RIGHT_90);
            if (i < diff) instructions.push_back(TURN_RIGHT_45);
        }

        double dist = L2_distance_squared(mouse_coordinate, next_coordinate);
        if (dist == 1.0) {
            instructions.push_back(MOVE_FORWARD);
        } else {
            instructions.push_back(MOVE_FORWARD_HALF);
        }

        mouse_direction = next_dir;
        mouse_coordinate = next_coordinate;
    }

    filter_turns(instructions);

    return instructions;
}

void Solver::accumulative_forward(double steps) {
    int full_steps = std::floor(steps);
    if (full_steps > 0) {
        m_mouse.moveForward(full_steps);
    }
    if (steps - (double)full_steps > 0.0) {
        m_mouse.moveForwardHalf();
    }
}

void Solver::run(const std::vector<Instruction>& instructions) {
    face(NORTH);
    double move_forward_for = 0.0;
    for (const Instruction& next_instruction : instructions) {
        switch (next_instruction) {
            case MOVE_FORWARD:
                move_forward_for += 1;
                break;
            case MOVE_FORWARD_HALF:
                move_forward_for += 0.5;
                break;
            case TURN_LEFT_45:
                accumulative_forward(move_forward_for);
                move_forward_for = 0;
                m_mouse.turnLeft45();
                break;
            case TURN_LEFT_90:
                accumulative_forward(move_forward_for);
                move_forward_for = 0;
                m_mouse.turnLeft();
                break;
            case TURN_RIGHT_45:
                accumulative_forward(move_forward_for);
                move_forward_for = 0;
                m_mouse.turnRight45();
                break;
            case TURN_RIGHT_90:
                accumulative_forward(move_forward_for);
                move_forward_for = 0;
                m_mouse.turnRight();
                break;
        }
    }
    accumulative_forward(move_forward_for);
}
