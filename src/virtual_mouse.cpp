#include "virtual_mouse.hpp"
#include "API.hpp"
#include "util.hpp"

void VirtualMouse::moveForward(int distance) {
    Mouse::moveForward(distance);
    API::moveForward(distance);
    API::setColor(getPosition().x, getPosition().y, 'G');
}
void VirtualMouse::turn(int angle) {
    if (angle == 90) API::turnLeft();
    else if (angle == -90) API::turnRight();
    // TODO: Some sort of logging indicating that turning non-90 is not possible in MMS
}

bool VirtualMouse::wallFront() {
    if (API::wallFront()) {
        API::setWall(getPosition().x, getPosition().y, dirToCardinalChar[getDirection()]);
        return true;
    }
    return false;

}
bool VirtualMouse::wallRight() {
    if (API::wallRight()) {
        API::setWall(getPosition().x, getPosition().y, dirToCardinalChar[rotate_right(getDirection())]);
        return true;
    }
    return false;
}
bool VirtualMouse::wallLeft() {
    if (API::wallLeft()) {
        API::setWall(getPosition().x, getPosition().y, dirToCardinalChar[rotate_left(getDirection())]);
        return true;
    }
    return false;
}

bool VirtualMouse::wasReset() {
    return API::wasReset();

}
void VirtualMouse::ackReset() {
    return API::ackReset();
}
