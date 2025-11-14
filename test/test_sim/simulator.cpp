#include <iostream>
#include "virtual_mouse.hpp"
#include "solver.hpp"

int main(int argc, char* argv[]) {
    VirtualMouse mouse;
    Maze maze;
    Solver solver(mouse, maze);

    maze.targets.push_back(Position(7, 7));
    maze.targets.push_back(Position(7, 8));
    maze.targets.push_back(Position(8, 7));
    maze.targets.push_back(Position(8, 8));

    solver.solve(); // Run from start to target

    maze.targets.clear();
    maze.targets.push_back(Position(0, 0));

    solver.solve(); // Run from target to start

}
