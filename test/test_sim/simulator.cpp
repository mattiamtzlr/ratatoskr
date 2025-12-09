#include <iostream>
#include <vector>

#include "API.hpp"
#include "config.hpp"
#include "solver.hpp"
#include "virtual_mouse.hpp"

using namespace Config;

int main(int argc, char* argv[]) {
    VirtualMouse mouse;
    Maze maze;
    Solver solver(mouse, maze);
    maze.set_targets(END_POINTS);

    solver.solve();  // Run from start to target

    maze.set_targets(START_POINTS);
    solver.solve();  // Run from target to start

    maze.set_targets(END_POINTS);
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
