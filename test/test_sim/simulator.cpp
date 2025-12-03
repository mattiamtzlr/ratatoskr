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
    std::vector<Instruction>* instr = new std::vector<Instruction>;
    std::vector<GraphCoordinate>* solved = new std::vector<GraphCoordinate>;
    solver.dijkstra(*solved);
    solver.parse_path(*solved, *instr);
    delete solved;
    solver.run(*instr);
    delete instr;
}
