#include "solver.hpp"

Solver::Solver(Mouse &mouse, Maze &maze) : m_mouse(mouse), m_maze(maze) {
    width = m_mouse.mazeWidth();
    height = m_mouse.mazeHeight();

    // walls[x][y][d], known[x][y][d], dist[x][y]
    walls = std::vector<std::vector<std::array<bool, 4>>>(
        width,
        std::vector<std::array<bool, 4>>(height, {false, false, false, false}));
    known = std::vector<std::vector<std::array<bool, 4>>>(
        width,
        std::vector<std::array<bool, 4>>(height, {false, false, false, false}));
}

void Solver::set_wall(int x, int y, Direction d, bool is_wall) {
    if (!m_maze.in_bounds(Position(x, y))) return;
    int nx = x + dx(d);
    int ny = y + dy(d);
    walls[x][y][d] = is_wall;
    known[x][y][d] = true;
    if (is_wall) m_mouse.setWall(x, y, dirToCardinalChar[d]);
    if (m_maze.in_bounds(Position(nx, ny))) {
        Direction od = rotate_half(d);
        walls[nx][ny][od] = is_wall;
        known[nx][ny][od] = true;
    }
};

// Flood-fill bfs_recompute (and paint numbers)
void Solver::bfs_recompute() {
    m_maze.reset_distances();

    std::deque<Position> q;
    for (Position &g : m_maze.targets) {
        m_maze.set_distance(g.x, g.y, 0);
        q.push_back(g);
    }

    while (!q.empty()) {
        Position pos = q.front();
        q.pop_front();
        for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
            int nx = pos.x + dx(d);
            int ny = pos.y + dy(d);
            if (!m_maze.in_bounds(Position(nx, ny))) continue;
            if (known[pos.x][pos.y][d] && walls[pos.x][pos.y][d]) continue;
            int nd = m_maze.get_distance(pos.x, pos.y) + 1;
            if (nd < m_maze.get_distance(nx, ny)) {
                m_maze.set_distance(nx, ny, nd);
                q.emplace_back(Position(nx, ny));
            }
        }
    }

    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            if (m_maze.get_distance(x, y) < INF)
                m_mouse.setText(x, y,
                                std::to_string(m_maze.get_distance(x, y)));
            else
                m_mouse.setText(x, y, "");
};

// Blue route preview
std::vector<std::pair<int, int>> Solver::compute_blue_route(int sx, int sy) {
    std::vector<std::pair<int, int>> route;
    std::set<std::pair<int, int>> seen;

    int x = sx;
    int y = sy;

    while (!m_maze.at_target(m_mouse.getPosition())) {
        int best_d = -1;
        int best_v = INF;

        for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
            int nx = x + dx(d), ny = y + dy(d);
            if (!m_maze.in_bounds(Position(nx, ny))) continue;
            if (known[x][y][d] && walls[x][y][d]) continue;
            if (m_maze.get_distance(nx, ny) < best_v) {
                best_v = m_maze.get_distance(nx, ny);
                best_d = d;
            }
        }
        x += dx((Direction)best_d);
        y += dy((Direction)best_d);
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
    int x = m_mouse.getPosition().x;
    int y = m_mouse.getPosition().y;

    set_wall(x, y, heading, m_mouse.wallFront());
    set_wall(x, y, rotate_left(heading), m_mouse.wallLeft());
    set_wall(x, y, rotate_right(heading), m_mouse.wallRight());
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

    // Goals (center)
    if (width % 2 == 0 && height % 2 == 0) {
        goals = {{width / 2 - 1, height / 2 - 1},
                 {width / 2, height / 2 - 1},
                 {width / 2 - 1, height / 2},
                 {width / 2, height / 2}};
    } else {
        goals = {{width / 2, height / 2}};
    }

    // Start
    visited.insert({x, y});
    bfs_recompute();
    paint_colors(visited, compute_blue_route(x, y));

    Direction heading = m_mouse.getDirection();
    Direction order_of_direction[4] = {heading, rotate_left(heading),
                                       rotate_right(heading),
                                       rotate_half(heading)};
    // Popluate the maze from the starting position
    detect_walls();
    bfs_recompute();
    // Run
    while (!m_maze.at_target(m_mouse.getPosition())) {
        int best_dir = -1;
        int best_val = INF;
        for (int k = 0; k < 4; ++k) {
            int next_x = x + dx(order_of_direction[k]);
            int next_y = y + dy(order_of_direction[k]);

            if (!m_maze.in_bounds(Position(next_x, next_y))) continue;

            if (known[x][y][order_of_direction[k]] &&
                walls[x][y][order_of_direction[k]])
                continue;

            if (m_maze.get_distance(next_x, next_y) < best_val) {
                best_val = m_maze.get_distance(next_x, next_y);
                best_dir = order_of_direction[k];
            }
        }

        face((Direction)best_dir);
        move_forward(x, y);
        paint_colors(visited, compute_blue_route(x, y));
        detect_walls();
        bfs_recompute();
    }
}

/* keep wrappers as-is */
void Solver::setWall(int x, int y, char direction) {
    m_maze.set_wall(x, y, cardinalCharToDir[direction]);
}
void Solver::clearWall(int x, int y, char direction) {
    m_maze.clear_wall(x, y, cardinalCharToDir[direction]);
}
int Solver::mazeWidth() { return m_maze.maze_width(); }
int Solver::mazeHeight() { return m_maze.maze_height(); }
