#include "solver.hpp"

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
    m_maze.visited.insert({m_mouse.getPosition().x, m_mouse.getPosition().y});
    while (!m_maze.at_target(m_mouse.getPosition())) {
        detect_and_set_walls();
        bfs();
        m_mouse.update_visuals(m_maze);
        Direction best_dir = NORTH;
        int best_val = UBOUND_DIST;
        for (Position neighbor :
             m_maze.valid_neighbors(m_mouse.getPosition())) {
            if (m_maze.get_distance(neighbor) < best_val) {
                best_val = m_maze.get_distance(neighbor);
                best_dir = dir_for_neighbor(neighbor, m_mouse.getPosition());
            }
        }
        face(best_dir);
        m_mouse.moveForward();
        m_maze.visited.insert(
            {m_mouse.getPosition().x, m_mouse.getPosition().y});
    }
}
