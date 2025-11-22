#include <iostream>

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

    std::vector<std::vector<Position>> diag_paths = maze.find_diagonal_paths(2);

    // Color diagonals of blue
    API::clearAllColor();
    for (const auto& path : diag_paths) {
        for (const Position& p : path) {
            API::setColor(p.x, p.y, 'b');
        }
    }

    maze.targets.clear();
    maze.targets.push_back(Position(7, 7));
    maze.targets.push_back(Position(7, 8));
    maze.targets.push_back(Position(8, 7));
    maze.targets.push_back(Position(8, 8));

    solver.finalize_discovery();

    // std::vector<Position> solved = solver.bfs_shortest_path(Position(0, 0));
    std::vector<Position> solved = solver.dijkstra(Position(0, 0));

    API::clearAllText();
    for (int i = 0; i < solved.size(); i++) {
	Position pos = solved[i];
        API::setText(pos.x, pos.y, std::to_string(i));
    }

    std::cerr << "Path Length: " << solved.size() << std::endl;
    API::clearAllColor();
    for (const Position& p : solved) {
        API::setColor(p.x, p.y, 'B');
    }
    solver.run(solved);
}
