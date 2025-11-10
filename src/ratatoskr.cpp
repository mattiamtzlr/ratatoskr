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
void Ratatoskr::turn(int angle) {
  time_to_turn = PERIOD / (360*angle) // relation using angle
  if (angle > 0) {                    // move right wheel forward, left wheel back
    m_motor_left.spin_cw(TURN_PWM);
    m_motor_right.spin_cw(TURN_PWM);
  } else {
    m_motor_left.spin_ccw(TURN_PWM);
    m_motor_right.spin_ccw(TURN_PWM);
  }
  delay(time_to_turn);
  stop();
}

/**
 * move @distance [one cell (16 cm)] forwards
 */
void Ratatoskr::moveForward(int distance) {
  time_forward = distance * TIME_PER_CELL;
  m_motor_left.spin_ccw(FORWARD_PWM);
  m_motor_right.spin_cw(FORWARD_PWM);
  delay(time_forward);
  stop();
    
}

void Ratatoskr::stop() {
  m_motor_left.stop();
  m_motor_right.stop();
}

//===============================[ SENSING ]====================================
bool Ratatoskr::wallFront() {}
bool Ratatoskr::wallRight() {}
bool Ratatoskr::wallLeft() {}
