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
    dist = std::vector<std::vector<int>>(width, std::vector<int>(height, INF));
}

char Solver::dir_char(int d) {
        static const char t[4] = {'n', 'e', 's', 'w'};
        return t[d];
}

char Solver::dir_letter(int d) {
    static const char t[4] = {'N', 'E', 'S', 'W'};
    return t[d];
}

int Solver::dx(int d) { return d == EAST ? 1 : (d == WEST ? -1 : 0); }
int Solver::dy(int d) { return d == NORTH ? 1 : (d == SOUTH ? -1 : 0); };
int Solver::opposite(int d) { return (d + 2) % 4; };

// member access ->
const char * Solver::bstr(bool v) { return v ? "True" : "False"; };

bool Solver::in_bounds(int x, int y) {
    return 0 <= x && x < width && 0 <= y && y < height;
};

void Solver::set_wall(int x, int y, int d, bool is_wall) {
    if (!in_bounds(x, y)) return;
    int nx = x + dx(d), ny = y + dy(d);
    walls[x][y][d] = is_wall;
    known[x][y][d] = true;
    if (is_wall)
        m_mouse.setWall(x, y, dir_char(d));
    else
        m_mouse.clearWall(x, y, dir_char(d));
    if (in_bounds(nx, ny)) {
        int od = opposite(d);
        walls[nx][ny][od] = is_wall;
        known[nx][ny][od] = true;
        if (is_wall)
            m_mouse.setWall(nx, ny, dir_char(od));
        else
            m_mouse.clearWall(nx, ny, dir_char(od));
    }
};

bool Solver::is_goal(int x, int y) {
    for (auto &g : goals)
        if (g.first == x && g.second == y) return true;
    return false;
};

// Flood-fill recompute (and paint numbers)
void Solver::recompute() {
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) dist[x][y] = INF;

    std::deque<std::pair<int, int>> q;
    for (auto &g : goals) {
        dist[g.first][g.second] = 0;
        q.push_back(g);
    }

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop_front();
        for (int d : {NORTH, EAST, SOUTH, WEST}) {
            int nx = cx + dx(d), ny = cy + dy(d);
            if (!in_bounds(nx, ny)) continue;
            if (known[cx][cy][d] && walls[cx][cy][d]) continue;
            int nd = dist[cx][cy] + 1;
            if (nd < dist[nx][ny]) {
                dist[nx][ny] = nd;
                q.emplace_back(nx, ny);
            }
        }
    }

    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
            if (dist[x][y] < INF)
                m_mouse.setText(x, y, std::to_string(dist[x][y]));
            else
                m_mouse.setText(x, y, "");
};

long long Solver::key(int a, int b) {
    return ((long long)a << 32) ^ (unsigned long long)b; // Why????
};

// Blue route preview
std::vector<std::pair<int, int>> Solver::compute_blue_route(int sx, int sy) {
    std::vector<std::pair<int, int>> route;
    std::set<long long> seen;


    int x = sx, y = sy;

    for (int step = 0; step < INF; ++step) {
        if (is_goal(x, y)) break;

        int best_d = -1, best_v = INF;

        for (int d : {NORTH, EAST, SOUTH, WEST}) {
            int nx = x + dx(d), ny = y + dy(d);
            if (!in_bounds(nx, ny)) continue;
            if (known[x][y][d] && walls[x][y][d]) continue;
            if (dist[nx][ny] < best_v) {
                best_v = dist[nx][ny];
                best_d = d;
            }
        }
        if (best_d == -1 || best_v >= dist[x][y]) break;
        x += dx(best_d);
        y += dy(best_d);
        if (seen.count(key(x, y))) break;
        seen.insert(key(x, y));
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

// Movement helpers with logs
int Solver::turn_left() {
    m_mouse.turnLeft();
    heading = (heading + 3) % 4;
    std::cerr << "Turn left with heading " << dir_letter(heading)
              << std::endl;
    return heading;
};
int Solver::turn_right() {
    m_mouse.turnRight();
    heading = (heading + 1) % 4;
    std::cerr << "Turn right with heading " << dir_letter(heading)
              << std::endl;
    return heading;
};

int Solver::face(int target_dir, int h) {
    int diff = (target_dir - h) % 4;
    if (diff < 0) diff += 4;
    if (diff == 1)
        h = turn_right();
    else if (diff == 2) {
        h = turn_right();
        h = turn_right();
    } else if (diff == 3)
        h = turn_left();
    return h;
};

void Solver::move_forward(int &cx, int &cy, int h) {
    m_mouse.moveForward(1);
    cx += dx(h);
    cy += dy(h);
    visited.insert({cx, cy});
    std::cerr << "Move to (" << cx << "," << cy << ") heading "
              << dir_letter(h) << std::endl;
};

// Sense & log
void Solver::sense(int cx, int cy, int h) {
    bool front = m_mouse.wallFront();
    bool left = m_mouse.wallLeft();
    bool right = m_mouse.wallRight();
    std::cerr << "Sensed @(" << cx << "," << cy << ") F:" << bstr(front)
              << " L:" << bstr(left) << " R:" << bstr(right) << std::endl;
    std::pair<int, bool> dir_map[3] = {{0, front}, {3, left}, {1, right}};
    for (auto &dir: dir_map) {
        int absolute_dir = (h + dir.first) % 4;
        set_wall(cx, cy, absolute_dir, dir.second);
    }
};

void Solver::solve() {
    // Directions (0=N,1=E,2=S,3=W)
    enum { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

    // Small lesson: In C++, "[&]" is a capture clause used in lambda
    // expressions that allows the lambda to access all variables from the
    // surrounding scope by reference. This means any changes made to those
    // variables inside the lambda will affect the original variables outside of
    // it.

    // auto allows lambdas, [&] gives access to references, (int d) is the
    // input, returns the output


    std::cerr << "Start. Maze size = " << width << " x " << height << std::endl;


    // i want to apologize for the thing you are going to see below, i just
    // wanted a 1 to 1 comparison to python bro, btw format fucks it uppp
    // Ali what am i looking at 😭😭😭  --Anthony



    // Outer borders
    for (int x = 0; x < width; ++x) {
        set_wall(x, 0, SOUTH, true);
        set_wall(x, height - 1, NORTH, true);
    }
    for (int y = 0; y < height; ++y) {
        set_wall(0, y, WEST, true);
        set_wall(width - 1, y, EAST, true);
    }

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
    recompute();
    paint_colors(visited, compute_blue_route(x, y));


    int order_of_direction[4] = {heading, (heading + 3) % 4, (heading + 1) % 4,
                                 (heading + 2) % 4};
    // Popluate the maze from the starting position
    sense(x, y, heading);
    recompute();
    // Run
    while (!is_goal(x, y)) {

        int best_dir = -1;
        int best_val = INF;
        for (int k = 0; k < 4; ++k) {
            int next_x = x + dx(order_of_direction[k]);
            int next_y = y + dy(order_of_direction[k]);

            if (!in_bounds(next_x, next_y)) continue;

            if (known[x][y][order_of_direction[k]] &&
                walls[x][y][order_of_direction[k]])
                continue;

            if (dist[next_x][next_y] < best_val) {
                best_val = dist[next_x][next_y];
                best_dir = order_of_direction[k];
            }
        }

        heading = face(best_dir, heading);
        move_forward(x, y, heading);
        paint_colors(visited, compute_blue_route(x, y));
        sense(x, y, heading);
        recompute();

        /* This is an advanced technique necessary in some mazes, superfluous
        for now
        // relax local minima
        if (best_dir == -1 || best_val >= dist[x][y]) {
            int m = INF;
            for (int d : {NORTH, EAST, SOUTH, WEST}) {
                int nx = x + dx(d);
                int ny = y + dy(d);
                if (!in_bounds(nx, ny)) continue;
                if (known[x][y][d] && walls[x][y][d]) continue;
                m = std::min(m, dist[nx][ny]);
            }
            if (m < INF) {
                dist[x][y] = m + 1;
                recompute();
                paint_colors(visited, compute_blue_route(x, y));
                std::cerr << "Relaxed current cell (local min)" << std::endl;
            }
            continue;
        }
        */
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
