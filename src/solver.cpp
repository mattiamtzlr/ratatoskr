#include "solver.hpp"

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <queue>
#include <tuple>

Solver::Solver(Mouse &mouse, Maze &maze) : m_mouse(mouse), m_maze(maze) {
    width = m_maze.maze_width();
    height = m_maze.maze_height();
    UBOUND_DIST = height * width + 1;
}

void Solver::face(Direction target_dir) {
    int diff = (target_dir - m_mouse.getDirection() + 4) % 4;
    if (diff == 3) {
        m_mouse.turnLeft();
    } else {
        for (int i = 0; i < diff; i++) m_mouse.turnRight();
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
    std::deque<Position> q;

    for (Position &g : m_maze.targets) {
        m_maze.set_distance(g, 0);
        q.push_back(g);
    }

    while (!q.empty()) {
        Position pos = q.front();
        q.pop_front();
        for (Position neighbor : m_maze.valid_neighbors(pos)) {
            int new_distance = m_maze.get_distance(pos) + 1;
            if (new_distance < m_maze.get_distance(neighbor)) {
                m_maze.set_distance(neighbor, new_distance);
                q.emplace_back(neighbor);
            }
        }
    }
};

void Solver::solve() {
    m_maze.set_border_walls();
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
        if (best_dir != curr_dir) {
            m_maze.turns.insert(pos_before);
        }
    }
}

std::vector<Position> Solver::dijkstra(Position start) {
    std::map<Position, std::vector<Edge>> adj_list = m_maze.get_adj_list();

    // distance and predecessor maps
    std::map<Position, int> dist;
    std::map<Position, Position> prev;
    const int INF = UBOUND_DIST;

    // initialize distances for all vertices (keys) and for all edge targets
    for (const auto &kv : adj_list) {
        dist[kv.first] = INF;
        for (const Edge &e : kv.second) {
            if (dist.find(e.target) == dist.end()) {
                dist[e.target] = INF;
            }
        }
    }

    // min-heap of (distance, position) with comparator on distance only
    struct PQCmp {
        bool operator()(const std::pair<int, Position> &a,
                        const std::pair<int, Position> &b) const {
            return a.first > b.first;
        }
    };

    std::priority_queue<std::pair<int, Position>,
                        std::vector<std::pair<int, Position>>, PQCmp>
        pq;

    dist[start] = 0;
    pq.push(std::make_pair(0, start));

    Position found_target = start;
    bool found = false;

    while (!pq.empty()) {
        auto top = pq.top();
        int d = top.first;
        Position u = top.second;
        std::cerr << "X: " << u.x << " Y: " << u.y << std::endl;
        pq.pop();

        // stale entry
        if (d > dist[u]) {
            continue;
        }

        if (m_maze.at_target(u)) {
            found_target = u;
            found = true;
            break;
        }

        auto it_adj = adj_list.find(u);
        if (it_adj == adj_list.end()) {
            continue;
        }

        for (const Edge &e : it_adj->second) {
            int alt = dist[u] + e.weight;
            if (alt < dist[e.target]) {
                dist[e.target] = alt;
                // update predecessor without requiring default-constructible
                // Position
                prev.erase(e.target);
                prev.insert(std::make_pair(e.target, u));
                pq.push(std::make_pair(alt, e.target));
            }
        }
    }

    std::vector<Position> path;
    if (!found) {
        std::cerr << "no path found" << std::endl;
        return path;  // no reachable target
    }

    // reconstruct path from start -> found_target
    Position cur = found_target;
    while (!(cur.x == start.x && cur.y == start.y)) {
        path.push_back(cur);
        auto it = prev.find(cur);
        if (it == prev.end()) {
            // predecessor missing, abort and return empty
            return std::vector<Position>();
        }
        cur = it->second;
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
std::vector<Position> Solver::bfs_shortest_path(Position start) {
    bfs();
    std::vector<Position> route;
    std::set<Position> seen;

    Position pos = start;

    while (!m_maze.at_target(pos)) {
        int best_v = m_maze.maze_height() * m_maze.maze_width() + 1;

        for (Position neighbor : m_maze.valid_neighbors(pos)) {
            if (m_maze.get_distance(neighbor) < best_v) {
                best_v = m_maze.get_distance(neighbor);
                pos = neighbor;
            }
        }
        if (seen.count(pos)) break;
        seen.insert(pos);
        route.emplace_back(pos);
    }
    return route;
}

void Solver::finalize_discovery() {
    // Assume all unchecked wall positions to have walls.
    for (int x = 0; x < MAZE_WIDTH; ++x) {
        for (int y = 0; y < MAZE_HEIGHT; ++y) {
            Position pos = Position(x, y);
            if (!m_maze.in_visited(pos)) {
                for (Position n : m_maze.valid_neighbors(pos)) {
                    if (!m_maze.in_visited(n))
                        m_maze.set_wall(n, dir_for_neighbor(pos, n));
                }
            }
        }
    }
    m_mouse.update_visuals(m_maze);
}
void Solver::run(std::vector<Position> solved) {
    for (Position next_position : solved) {
        if (next_position.x == m_mouse.getPosition().x &&
            next_position.y == m_mouse.getPosition().y)
            continue;

        face(dir_for_neighbor(next_position, m_mouse.getPosition()));
        m_mouse.moveForward();
    }
}
