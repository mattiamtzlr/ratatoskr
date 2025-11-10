#include "ratatoskr.hpp"

Ratatoskr::Ratatoskr(Maze &maze, GearMotor &motor_left, GearMotor &motor_right,
                     ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
                     ToF &tof_right, MPU6050 &gyro, LEDMatrix &screen)
    : Mouse(maze),
      m_motor_left(motor_left),
      m_motor_right(motor_right),
      m_tof_left(tof_left),
      m_tof_front_left(tof_front_left),
      m_tof_front_right(tof_front_right),
      m_tof_right(tof_right),
      m_gyro(gyro),
      m_screen(screen) {}

//===============================[ CONTROL ]====================================
/**
 * turn @angle radians in counterclockwise direction
 */
void Ratatoskr::turn(int angle) {}

/**
 * move @distance [units] forwards
 */
void Ratatoskr::moveForward(int distance) {
    // convert distance to rpm
    m_motor_left.spin_ccw(100);
    m_motor_right.spin_cw(100);
    delay(1000);
    m_motor_left.stop();
    m_motor_right.stop();
}

//===============================[ SENSING ]====================================
bool Ratatoskr::wallFront() {}
bool Ratatoskr::wallRight() {}
bool Ratatoskr::wallLeft() {}
