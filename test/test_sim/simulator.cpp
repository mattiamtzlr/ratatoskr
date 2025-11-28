#include <iostream>
#include <vector>

#include "API.hpp"
#include "solver.hpp"
#include "virtual_mouse.hpp"

int main(int argc, char* argv[]) {
    VirtualMouse mouse;
    Maze maze;
    Solver solver(mouse, maze);

    maze.targets.push_back(Position(2, 2));

    solver.solve();  // Run from start to target

    maze.targets.clear();
    maze.targets.push_back(Position(0, 0));

    solver.solve();  // Run from target to start

    maze.targets.clear();
    maze.targets.push_back(Position(2, 2));

    solver.finalize_discovery();
    /*
    std::map<GraphCoordinate, std::set<Edge>> adj = maze.get_adj_list();
    for (std::pair<GraphCoordinate, std::set<Edge>> kv : adj) {
        std::cerr << "x: " << kv.first.x << " y:" << kv.first.y << std::endl;
        for (Edge e : kv.second) {
            std::cerr << "- x: " << e.target.x << " y:" << e.target.y
                      << std::endl;
        }
    }
    */

    std::vector<GraphCoordinate> solved = solver.dijkstra(GraphCoordinate());

    std::vector<std::vector<Position>> diag_paths = {};
    // maze.find_diagonal_paths(2, solved);

    std::vector<char> colors = {'r', 'g', 'b', 'y', 'R', 'G'};
    // Color diagonals of blue
    for (const GraphCoordinate& p : solved) {
        std::cerr << "x: " << p.x << " y:" << p.y << std::endl;
    }

    solver.run(solved, diag_paths);
}
