#include "virtual_mouse.hpp"

void VirtualMouse::moveForward(int distance) {
    API::moveForward(distance);
}
void VirtualMouse::turn(int angle) {
    if (angle == 90) API::turnLeft();
    else if (angle == -90) API::turnRight();
}

bool VirtualMouse::wallFront() {
    return API::wallFront();

}
bool VirtualMouse::wallRight() {
    return API::wallRight();

}
bool VirtualMouse::wallLeft() {
    return API::wallLeft();
}

bool VirtualMouse::wasReset() {
    return API::wasReset();

}
void VirtualMouse::ackReset() {
    return API::ackReset();
}
