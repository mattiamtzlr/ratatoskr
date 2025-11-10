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
 * 
 * due to the structure of the drivetrain, one wheel moves forwards when
 * the motor is driven ccw and the other moves backwards when its motor is
 * driven ccw, idem for cw
 */
void Ratatoskr::turn(int angle) {
    // TODO: use gyro to close the loop; for now, differential spin.
    float time_to_turn = PERIOD / (360 * angle);  // relation using angle
    if (angle > 0) {  // move right wheel forward, left wheel back
        m_motor_left.spin_cw(TURN_PWM);
        m_motor_right.spin_cw(TURN_PWM);
    } else {
        m_motor_left.spin_ccw(TURN_PWM);
        m_motor_right.spin_ccw(TURN_PWM);
    }
    // timing/gyro stop goes here
    delay(time_to_turn);
    stop();
}

/**
 * move @distance [one cell (16 cm)] forwards
 */
void Ratatoskr::moveForward(int distance) {
    // TODO: PID on encoders + wall following; for now, spin both forward briefly.
    Mouse::moveForward(distance);  // Don't remove this, this updates the
                                   // position of the mouse in the maze!
    float time_forward = distance * TIME_PER_CELL;
    m_motor_left.spin_ccw(FORWARD_PWM);
    m_motor_right.spin_cw(FORWARD_PWM);
    // timing/odometry goes here
    delay(time_forward);
    stop();
}

void Ratatoskr::stop() {
    m_motor_left.stop();
    m_motor_right.stop();
}

//===============================[ SENSING
//]====================================
// Below 15 cm (or 10)
/**
 * Front wall if either front-left or front-right ToF sees something closer
 * than FRONT_WALL_MM. Ignores zero readings (sensor not ready).
 */
bool Ratatoskr::wallFront() {
    const uint16_t d_fl = m_tof_front_left.read();
    const uint16_t d_fr = m_tof_front_right.read();
    const bool fl = (d_fl > 0) && (d_fl < FRONT_WALL_MM);
    const bool fr = (d_fr > 0) && (d_fr < FRONT_WALL_MM);
    return fl || fr;
}


/**
 * Right wall if right ToF < SIDE_WALL_MM (ignoring zeros).
 */
bool Ratatoskr::wallRight() {
    const uint16_t d_r = m_tof_right.read();
    return (d_r > 0) && (d_r < SIDE_WALL_MM);
}

/**
 * Left wall if left ToF < SIDE_WALL_MM (ignoring zeros).
 */
bool Ratatoskr::wallLeft() {
    const uint16_t d_l = m_tof_left.read();
    return (d_l > 0) && (d_l < SIDE_WALL_MM);
}
