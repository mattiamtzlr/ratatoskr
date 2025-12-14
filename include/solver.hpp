#pragma once

#include <vector>

#include "maze.hpp"
#include "mouse.hpp"
#include "util.hpp"

class Solver {
   private:
    Mouse &m_mouse;
    Maze &m_maze;

    int width;
    int height;

    int UBOUND_DIST;

    void bfs();
    void face(Direction target_dir);
    void detect_and_set_walls();
    void accumulative_forward(double steps);
    void filter_turns(std::vector<Instruction> &instr);

   public:
    Solver(Mouse &mouse, Maze &maze);

    /**
     * Run floodfill until reaching the target.
     */
    void solve();

    /**
     * Assume that all walls that no information was
     * gathered on do exits.
     */
    void finalize_discovery();

    /**
     * Run Dijkstra's algorithm on the maze.
     * If diagonals is set to false, no diagonals.
     * Requires the user to pass a vector to populate.
     */
    std::vector<GraphCoordinate> &dijkstra(std::vector<GraphCoordinate> &path,
                                           bool diagonals = true);

    /**
     * Parse the vector of coordinates into a vector of
     * instructions.
     * Requires the user to pass a vector to populate.
     */
    std::vector<Instruction> &parse_path(
        std::vector<GraphCoordinate> &path,
        std::vector<Instruction> &instructions);

    /**
     * Run the instruction vector on the mouse.
     * Will accumulate instructions where possible.
     */
    void run(const std::vector<Instruction> &instructions);
};
