#include "virtual_mouse.hpp"

#include <iostream>
#include <vector>
#include <cmath>

#include "API.hpp"
#include "util.hpp"

using namespace Config;

void VirtualMouse::moveForward(float distance) {
    int full_steps = std::floor(distance);
    if (full_steps > 0) {
        API::moveForward(full_steps);
    }
    if (distance - (double)full_steps > 0.0) {
        API::moveForwardHalf();
    }
}

void VirtualMouse::moveForward(int distance) {
    Mouse::moveForward(distance);
    API::moveForward(distance);
    API::setColor(getPosition().x, getPosition().y, 'G');
}

void VirtualMouse::moveForwardHalf(int num_half_steps) {
    Mouse::moveForwardHalf(num_half_steps);
    API::moveForwardHalf(num_half_steps);
    API::setColor(getPosition().x, getPosition().y, 'G');
}

void VirtualMouse::turn(int angle) {
    if (angle == 90)
        API::turnLeft();
    else if (angle == -90)
        API::turnRight();
    else if (angle == 45)
        API::turnLeft45();
    else if (angle == -45)
        API::turnRight45();
    else if (angle == 180) {
        API::turnLeft();
        API::turnLeft();
    }
    // TODO: Some sort of logging indicating that turning non-90 is not possible
    // in MMS
    // NOTE: YES IT IS THERE IS API::TURN45DEGREES
}

bool VirtualMouse::wallFront() {
    if (API::wallFront()) {
        API::setWall(getPosition().x, getPosition().y,
                     dirToCardinalChar[getDirection()]);
        return true;
    }
    return false;
}
bool VirtualMouse::wallRight() {
    if (API::wallRight()) {
        API::setWall(getPosition().x, getPosition().y,
                     dirToCardinalChar[rotate_right(getDirection())]);
        return true;
    }
    return false;
}
bool VirtualMouse::wallLeft() {
    if (API::wallLeft()) {
        API::setWall(getPosition().x, getPosition().y,
                     dirToCardinalChar[rotate_left(getDirection())]);
        return true;
    }
    return false;
}
void VirtualMouse::log(std::string msg) { std::cerr << msg << std::endl; }

void VirtualMouse::update_visuals(Maze& maze) {
    for (int x = 0; x < MAZE_WIDTH; ++x) {
        for (int y = 0; y < MAZE_HEIGHT; ++y) {
            for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
                if (maze.exists_wall(Position(x, y), d)) {
                    API::setWall(x, y, dirToCardinalChar[d]);
                }
            }
            if (maze.get_distance(Position(x, y)) <
                MAZE_HEIGHT * MAZE_WIDTH + 1) {
                API::setText(x, y,
                             std::to_string(maze.get_distance(Position(x, y))));
            } else {
                API::setText(x, y, "");
            }
        }
    }
    std::vector<Position> route;
    std::set<Position> seen;

    Position pos = getPosition();

    while (!maze.at_target(pos)) {
        int best_v = MAZE_HEIGHT * MAZE_WIDTH + 1;

        for (Position neighbor : maze.valid_neighbors(pos)) {
            if (maze.get_distance(neighbor) < best_v) {
                best_v = maze.get_distance(neighbor);
                pos = neighbor;
            }
        }
        if (seen.count(pos)) break;
        seen.insert(pos);
        route.emplace_back(pos);
    }
    API::clearAllColor();
    for (auto& p : maze.visited) API::setColor(p.x, p.y, 'G');
    for (auto& p : route) API::setColor(p.x, p.y, 'B');
}

bool VirtualMouse::wasReset() { return API::wasReset(); }
void VirtualMouse::VirtualMouse::ackReset() { return API::ackReset(); }
