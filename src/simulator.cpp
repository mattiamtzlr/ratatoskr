#include <iostream>
#include "virtual_mouse.hpp"
#include "solver.hpp"

int main(int argc, char* argv[]) {
    VirtualMouse mouse;
    Maze maze;
    Solver solver(mouse, maze);
    solver.solve();
}
