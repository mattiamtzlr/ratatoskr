#include <iostream>
#include <vector>

#include "API.hpp"
#include "solver.hpp"
#include "virtual_mouse.hpp"

int main(int argc, char* argv[]) {
    VirtualMouse mouse;
    Maze maze;
    Solver solver(mouse, maze);

    maze.targets.push_back(Position(7, 7));
    maze.targets.push_back(Position(7, 8));
    maze.targets.push_back(Position(8, 7));
    maze.targets.push_back(Position(8, 8));

    solver.solve();  // Run from start to target

    maze.targets.clear();
    maze.targets.push_back(Position(0, 0));

    solver.solve();  // Run from target to start

    maze.targets.clear();
    maze.targets.push_back(Position(7, 7));
    maze.targets.push_back(Position(7, 8));
    maze.targets.push_back(Position(8, 7));
    maze.targets.push_back(Position(8, 8));

    solver.finalize_discovery();
    std::map<GraphCoordinate, std::set<Edge>> adj = maze.get_adj_list();
    for (std::pair<GraphCoordinate, std::set<Edge>> kv : adj) {
        std::cerr << "x: " << kv.first.x << " y:" << kv.first.y << std::endl;
        for (Edge e : kv.second) {
            std::cerr << "- x: " << e.target.x << " y:" << e.target.y
                      << std::endl;
        }
    }

    std::vector<GraphCoordinate> solved = solver.dijkstra(GraphCoordinate());


    std::vector<char> colors = {'r', 'g', 'b', 'y', 'R', 'G'};
    // Color diagonals of blue
    API::clearAllColor();
    for (const GraphCoordinate& p : solved) {
        API::setColor((int)p.x, (int)p.y, 'B');
    }
    int color = 0;
    for (const auto& path : diag_paths) {
        for (const Position& p : path) {
            API::setColor(p.x, p.y, colors[color]);
        }
        color = (color + 1) % 6;
    }

    API::clearAllText();
    for (int i = 0; i < solved.size(); i++) {
        GraphCoordinate pos = solved[i];
        API::setText((int)pos.x, (int)pos.y, std::to_string(i));
    }
    solver.run(solved, diag_paths);
}
