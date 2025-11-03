#include <iostream>
#include <string>

#include "API.hpp"

enum Cardinal { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

Cardinal current_heading;

void log(const std::string& text) { std::cerr << text << std::endl; }

void turn_cardinal(Cardinal target) {
    int diff = (target - current_heading) % 4;
    if (diff == 3) {
        API::turnLeft();
    } else {
        for (int i = 0; i < diff; i++) {
            API::turnRight();
        }
    }
    current_heading = target;
}

void move_forward() { API::moveForward(); }

void detect_walls() {
    bool wall_front = API::wallFront();
    bool wall_left = API::wallLeft();
    bool wall_right = API::wallRight();
}

void solve() {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "start");
    while (true) {
        if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();
    }
}

int main(int argc, char* argv[]) {
    // Mouse sim_mouse = Mouse(API_MODE);
    // Solver sim_solver = Solver(sim_mouse);
    /*sim_solver.*/ solve();
}
