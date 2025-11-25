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
    int diff = (target_dir - m_mouse.getDirection() + 8) % 8;
    if (diff >= 5) {
        for(int i = 0; i < 8 - diff; i++) m_mouse.turnLeft45();
    } else {
        for (int i = 0; i < diff; i++) m_mouse.turnRight45();
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

    // prio queue of (distance, position) with comparator on distance only
    p_queue pq;

    // distance and predecessor maps
    std::map<Position, int> dist;
    std::map<Position, Position> prev;

    // initialize distances for all vertices (keys) and for all edge targets
    for (std::pair<Position, std::vector<Edge>> kv : adj_list)
        dist[kv.first] = UBOUND_DIST;

    dist[start] = 0;
    pq.push(std::make_pair(0, start));

    Position found_target = start;
    bool found = false;

    while (!pq.empty()) {
        const std::pair<int, Position> top = pq.top();
        int d = top.first;
        Position u = top.second;
        pq.pop();

        if (m_maze.at_target(u)) {
            found_target = u;
            found = true;
            break;
        }

        std::vector<Edge> edges = adj_list[u];

        for (Edge e : edges) {
            int alt = dist[u] + e.weight /* + Vertex weight*/;
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

    // reconstruct path from start -> found_target
    Position cur = found_target;
    while (!(cur.x == start.x && cur.y == start.y)) {
        path.push_back(cur);
        cur = prev[cur];
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
    /* Assume all unchecked wall positions to have walls. */
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
        Position curr_position = m_mouse.getPosition();
        std::vector<Position> diag_neighbors = m_maze.valid_diag_neighbors(curr_position); 
        if(std::find(diag_neighbors.begin(), diag_neighbors.end(), next_position) != diag_neighbors.end()){
            if(next_position.x > curr_position.x &&
               next_position.y > curr_position.y){
                std::cerr << "Wants to turn diag NE" << std::endl;
                if(m_maze.exists_wall(curr_position, NORTH)){
                    std::cerr << "Wall to the NORTH, going EAST first" << std::endl;
                    face(EAST);
                    m_mouse.moveForwardHalf();
                    face(NORTH_EAST);
                }
                else{
                    std::cerr << "No wall to the NORTH, going NORTH first" << std::endl;
                    face(NORTH);
                    m_mouse.moveForwardHalf();
                    face(NORTH_EAST);
                }
            }
            else if(next_position.x > m_mouse.getPosition().x &&
                    next_position.y < m_mouse.getPosition().y){
                face(SOUTH_EAST);
            }
            else if(next_position.x < m_mouse.getPosition().x &&
                    next_position.y < m_mouse.getPosition().y){
                face(SOUTH_WEST);
            }
            else if(next_position.x < m_mouse.getPosition().x &&
                    next_position.y > m_mouse.getPosition().y){
                face(NORTH_WEST);
            }
        }
        else{
            face(dir_for_neighbor(next_position, m_mouse.getPosition()));
            m_mouse.moveForward();
        }
    }
}
