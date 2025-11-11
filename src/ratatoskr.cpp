#include "ratatoskr.hpp"
#define PERIOD 2000.0
#define TIME_PER_CELL 2000.0
Ratatoskr::Ratatoskr(GearMotor &motor_left, GearMotor &motor_right,
                     ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
                     ToF &tof_right /*, MPU6050 &gyro, LEDMatrix &screen*/)
    : Mouse(),
      m_motor_left(motor_left),
      m_motor_right(motor_right),
      m_tof_left(tof_left),
      m_tof_front_left(tof_front_left),
      m_tof_front_right(tof_front_right),
      m_tof_right(tof_right) /*,
       m_gyro(gyro),
       m_screen(screen)*/
{}

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
    delay(time_to_turn);
    stop();
}

/**
 * move @distance [one cell (16 cm)] forwards
 */
void Ratatoskr::moveForward(int distance) {
    // TODO: PID on encoders + wall following; for now, spin both forward
    // briefly.
    Mouse::moveForward(distance);  // Don't remove this, this updates the
                                   // position of the mouse in the maze!
    float time_forward = distance * TIME_PER_CELL;
    u_int8_t pwm_left = FORWARD_PWM;
    u_int8_t pwm_right = FORWARD_PWM;

    for (int i = 0; i < 10; i++) {
        if (m_motor_left.get_rpm() > m_motor_right.get_rpm()) {
            pwm_right += 10;
            pwm_left -= 10;
        }
        if (m_motor_left.get_rpm() < m_motor_right.get_rpm()) {
            pwm_right -= 10;
            pwm_left += 10;
        }
        m_motor_left.spin_ccw(pwm_left);
        m_motor_right.spin_cw(pwm_right);
        // timing/odometry goes here
        delay(time_forward / 10);
    }
    stop();
}
/**
 * SLAM THE BRAKES!
 */
void Ratatoskr::stop() {
    m_motor_left.stop();
    m_motor_right.stop();
}

//===============================[ SENSING ]====================================
// Below 15 cm (or 10)
/**
 * Front wall if either front-left or front-right ToF sees something closer
 * than FRONT_WALL_MM. Ignores zero readings (sensor not ready).
 */
bool Ratatoskr::wallFront() {
    uint16_t distance_front_left = m_tof_front_left.read();
    uint16_t distance_front_right = m_tof_front_right.read();
    return (distance_front_left > 0) && (distance_front_left < FRONT_WALL_MM) ||
           (distance_front_right > 0) && (distance_front_right < FRONT_WALL_MM);
}

/**
 * Detect if a wall is towards the right.
 */
bool Ratatoskr::wallRight() {
    uint16_t distance_right = m_tof_right.read();
    return (distance_right > 0) && (distance_right < SIDE_WALL_MM);
}

/**
 * Detect if a wall is towards the left.
 */
bool Ratatoskr::wallLeft() {
    uint16_t distance_left = m_tof_left.read();
    return (distance_left > 0) && (distance_left < SIDE_WALL_MM);
}
void Ratatoskr::setWall(int x, int y, char direction) {}
void Ratatoskr::clearWall(int x, int y, char direction) {}

void Ratatoskr::setColor(int x, int y, char color) {}
void Ratatoskr::clearColor(int x, int y) {}
void Ratatoskr::clearAllColor() {}

void Ratatoskr::setText(int x, int y, const std::string &text) {}
void Ratatoskr::clearText(int x, int y) {}
void Ratatoskr::clearAllText() {}

bool Ratatoskr::wasReset() { return false; }
void Ratatoskr::ackReset() {}
