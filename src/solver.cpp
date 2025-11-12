#include "solver.hpp"

Solver::Solver(Mouse &mouse, Maze &maze) : m_mouse(mouse), m_maze(maze) {
    width = m_mouse.mazeWidth();
    height = m_mouse.mazeHeight();

    known = std::vector<std::vector<std::array<bool, 4>>>(
        width,
        std::vector<std::array<bool, 4>>(height, {false, false, false, false}));
}

void Solver::set_wall(Position pos, Direction d, bool is_wall) {
    if (!m_maze.in_bounds(pos)) return;
    Position neighbor = get_neighbor(pos, d);
    if (is_wall) m_maze.set_wall(pos, d);
    known[pos.x][pos.y][d] = true;
    if (is_wall) m_mouse.setWall(pos.x, pos.y, dirToCardinalChar[d]);
    if (m_maze.in_bounds(neighbor)) {
        Direction od = rotate_half(d);
        if (is_wall) m_maze.set_wall(neighbor, od);
        known[neighbor.x][neighbor.y][od] = true;
    }
};

// Flood-fill bfs_recompute (and paint numbers)
void Solver::bfs_recompute() {
    m_maze.reset_distances();
    std::deque<Position> q;

    for (Position &g : m_maze.targets) {
        m_maze.set_distance(g, 0);
        q.push_back(g);
    }

    while (!q.empty()) {
        Position pos = q.front();
        q.pop_front();
        for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
            Position neighbor = get_neighbor(pos, d);
            if (!m_maze.in_bounds(neighbor)) continue;
            if (known[pos.x][pos.y][d] && m_maze.exists_wall(pos, d)) continue;
            int nd = m_maze.get_distance(pos) + 1;
            if (nd < m_maze.get_distance(neighbor)) {
                m_maze.set_distance(neighbor, nd);
                q.emplace_back(neighbor);
            }
        }
    }
};
void Solver::update_text() {
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            if (m_maze.get_distance(Position(x, y)) < INF)
                m_mouse.setText(
                    x, y, std::to_string(m_maze.get_distance(Position(x, y))));
            else
                m_mouse.setText(x, y, "");
}

// Blue route preview
std::vector<std::pair<int, int>> Solver::compute_blue_route(int sx, int sy) {
    std::vector<std::pair<int, int>> route;
    std::set<std::pair<int, int>> seen;


    Position pos = Position(sx, sy);

    while (!m_maze.at_target(m_mouse.getPosition())) {
        Direction best_d = NORTH;
        int best_v = INF;

        for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
            Position neighbor = get_neighbor(pos, d);
            if (!m_maze.in_bounds(neighbor)) continue;
            if (known[x][y][d] && m_maze.exists_wall(pos, d)) continue;
            if (m_maze.get_distance(neighbor) < best_v) {
                best_v = m_maze.get_distance(neighbor);
                best_d = d;
            }
        }
        pos.x += dx(best_d);
        pos.y += dy(best_d);
        std::pair<int, int> coords = std::make_pair(x, y);
        if (seen.count(coords)) break;
        seen.insert(coords);
        route.emplace_back(x, y);
    }
    return route;
};

// Paint overlays
void Solver::paint_colors(const std::set<std::pair<int, int>> &visited,
                          const std::vector<std::pair<int, int>> &blue) {
    m_mouse.clearAllColor();
    for (auto &p : visited) m_mouse.setColor(p.first, p.second, 'G');
    for (auto &p : blue) m_mouse.setColor(p.first, p.second, 'B');
};

void Solver::face(Direction target_dir) {
    int diff = (target_dir - m_mouse.getDirection() + 4) % 4;
    if (diff == 3) {
        m_mouse.turnLeft();
    } else {
        for (int i = 0; i < diff; i++) {
            m_mouse.turnRight();
        }
    }
}

void Solver::move_forward(int &cx, int &cy) {
    Direction h = m_mouse.getDirection();
    m_mouse.moveForward(1);
    cx += dx(h);
    cy += dy(h);
    visited.insert({cx, cy});
};

// detect_walls & log
void Solver::detect_walls() {
    Direction heading = m_mouse.getDirection();
    Position pos = m_mouse.getPosition();

    set_wall(pos, heading, m_mouse.wallFront());
    set_wall(pos, rotate_left(heading), m_mouse.wallLeft());
    set_wall(pos, rotate_right(heading), m_mouse.wallRight());
}

void Solver::solve() {
    /* =====> DO THIS IN THE MAZE CONSTRUCTOR
    for (int x = 0; x < width; ++x) {
        set_wall(x, 0, SOUTH, true);
        set_wall(x, height - 1, NORTH, true);
    }
    for (int y = 0; y < height; ++y) {
        set_wall(0, y, WEST, true);
        set_wall(width - 1, y, EAST, true);
    }
    */

    // Start
    visited.insert({x, y});
    bfs_recompute();
    update_text();
    paint_colors(visited, compute_blue_route(x, y));

    Direction heading = m_mouse.getDirection();
    // Popluate the maze from the starting position
    detect_walls();
    bfs_recompute();
    update_text();
    // Run
    while (!m_maze.at_target(m_mouse.getPosition())) {
        int best_dir = -1;
        int best_val = INF;
        for (Direction k : {NORTH, EAST, SOUTH, WEST}) {
            Position neighbor = get_neighbor(m_mouse.getPosition(), k);

            if (!m_maze.in_bounds(neighbor)) continue;

            if (known[x][y][k] && m_maze.exists_wall(Position(x,y), k)) continue;

            if (m_maze.get_distance(neighbor) < best_val) {
                best_val = m_maze.get_distance(neighbor);
                best_dir = k;
            }
        }

        face((Direction)best_dir);
        move_forward(x, y);
        paint_colors(visited, compute_blue_route(x, y));
        detect_walls();
        bfs_recompute();
        update_text();
    }
}

/* keep wrappers as-is */
void Solver::setWall(int x, int y, char direction) {
    m_maze.set_wall(Position(x, y), cardinalCharToDir[direction]);
}
void Solver::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(Position(x, y), cardinalCharToDir[direction]);
}
int Solver::mazeWidth() { return m_maze.maze_width(); }
int Solver::mazeHeight() { return m_maze.maze_height(); }
