#include "virtual_mouse.hpp"

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
void VirtualMouse::update_visuals(Maze& maze) {
    for (int x = 0; x < maze.maze_width(); ++x)
        for (int y = 0; y < maze.maze_height(); ++y)
            if (maze.get_distance(Position(x, y)) <
                maze.maze_height() * maze.maze_width() + 1)
                API::setText(x, y,
                             std::to_string(maze.get_distance(Position(x, y))));
            else
                API::setText(x, y, "");

    std::vector<std::pair<int, int>> route;
    std::set<std::pair<int, int>> seen;

    Position pos = getPosition();

    while (!maze.at_target(pos)) {
        Direction best_d = NORTH;
        int best_v = maze.maze_height() * maze.maze_width() + 1;

        for (Direction d : {NORTH, EAST, SOUTH, WEST}) {
            Position neighbor = get_neighbor(pos, d);
            if (!maze.in_bounds(neighbor) || maze.exists_wall(pos, d)) {
                continue;
            }
            if (maze.get_distance(neighbor) < best_v) {
                best_v = maze.get_distance(neighbor);
                best_d = d;
            }
        }
        pos.x += dx(best_d);
        pos.y += dy(best_d);

        std::pair<int, int> coords = std::make_pair(pos.x, pos.y);
        if (seen.count(coords)) break;
        seen.insert(coords);
        route.emplace_back(coords.first, coords.second);
    }
    API::clearAllColor();
    for (auto& p : maze.visited) API::setColor(p.first, p.second, 'G');
    for (auto& p : route) API::setColor(p.first, p.second, 'B');
}



bool VirtualMouse::wasReset() { return API::wasReset(); }
void VirtualMouse::VirtualMouse::ackReset() { return API::ackReset(); }
