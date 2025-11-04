#include <iostream>
#include <map>
#include <string>

#include "API.hpp"

enum Cardinal { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };
typedef struct _Position {
    int x;
    int y;
} Position;

std::map<Cardinal, char> cardinalToChar = {
    {NORTH, 'n'}, {EAST, 'e'}, {SOUTH, 's'}, {WEST, 'w'}};

Cardinal current_heading = NORTH;
Position current_position = {0, 0};
Position goals[4] = {{7, 7}, {8, 7}, {7, 8}, {8, 8}};

bool in_goals() {
    for (int i = 0; i < 4; i++) {
        if (goals[i].x == current_position.x &&
            goals[i].y == current_position.y) {
            return true;
        }
    }
    return false;
}

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

void move_forward() {
    API::moveForward();
    current_position.y += ((current_heading == NORTH)   ? 1
                           : (current_heading == SOUTH) ? -1
                                                        : 0);
    current_position.x += ((current_heading == EAST)   ? 1
                           : (current_heading == WEST) ? -1
                                                       : 0);
    API::setColor(current_position.x, current_position.y, 'G');
}

void set_wall(Cardinal direction, bool wall_detected) {
    if (wall_detected) {
        API::setWall(current_position.x, current_position.y,
                     cardinalToChar[direction]);
    } else {
        API::clearWall(current_position.x, current_position.y,
                       cardinalToChar[direction]);
    }
}

void detect_walls() {
    set_wall((Cardinal)((current_heading) % 4), API::wallFront());
    set_wall((Cardinal)((current_heading + 3) % 4), API::wallLeft());
    set_wall((Cardinal)((current_heading + 1) % 4), API::wallRight());
}

void solve() {
    log("Running...");

    API::setColor(0, 0, 'G');
    API::setText(0, 0, "start");
    while (!in_goals()) {
        detect_walls();
        if (!API::wallLeft()) {
            API::turnLeft();
            current_heading = (Cardinal)((current_heading + 3) % 4);
        }
        while (API::wallFront()) {
            API::turnRight();
            current_heading = (Cardinal)((current_heading + 1) % 4);
        }
        move_forward();
    }
}

int main(int argc, char* argv[]) {
    // Mouse sim_mouse = Mouse(API_MODE);
    // Solver sim_solver = Solver(sim_mouse);
    /*sim_solver.*/ solve();
}
