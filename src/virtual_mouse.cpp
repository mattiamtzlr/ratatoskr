#include "virtual_mouse.hpp"

#include <iostream>
#include <vector>

#include "API.hpp"
#include "util.hpp"

void VirtualMouse::moveForward(int distance) {
    Mouse::moveForward(distance);
    API::moveForward(distance);
    API::setColor(getPosition().x, getPosition().y, 'G');
}
void VirtualMouse::turn(int angle) {
    if (angle == 90)
        API::turnLeft();
    else if (angle == -90)
        API::turnRight();
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
    for (int x = 0; x < maze.maze_width(); ++x)
        for (int y = 0; y < maze.maze_height(); ++y)
            if (maze.get_distance(Position(x, y)) <
                maze.maze_height() * maze.maze_width() + 1)
                API::setText(x, y,
                             std::to_string(maze.get_distance(Position(x, y))));
            else
                API::setText(x, y, "");

    std::vector<Position> route;
    std::set<Position> seen;

    Position pos = getPosition();

    while (!maze.at_target(pos)) {
        int best_v = maze.maze_height() * maze.maze_width() + 1;

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
