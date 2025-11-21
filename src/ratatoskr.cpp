#include "ratatoskr.hpp"

#include <string>

#include "pid.hpp"

Ratatoskr::Ratatoskr(GearMotor &motor_left, GearMotor &motor_right,
                     ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
                     ToF &tof_right, MPU6050 &gyro)
    : Mouse(),
      m_motor_left(motor_left),
      m_motor_right(motor_right),
      m_tof_left(tof_left),
      m_tof_front_left(tof_front_left),
      m_tof_front_right(tof_front_right),
      m_tof_right(tof_right),
      m_gyro(gyro) {}

void Ratatoskr::calibrateEncoders() {
    stop();
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    log("=== Encoder calibration mode ===");
    log("Move the mouse manually. Press Enter in Serial Monitor when done.");

    unsigned long last_print = 0;
    while (true) {
        long left_encoder = m_motor_left.get_encoder_count();
        long right_encoder = m_motor_right.get_encoder_count();

        unsigned long now = millis();
        if (now - last_print >= 100) {
            last_print = now;
            log("Ticks L = " + std::to_string(left_encoder) +
                " R = " + std::to_string(right_encoder));
        }

        if (Serial.available() > 0) {
            int c = Serial.read();
            if (c == '\n' || c == '\r') {
                log("\nCalibration finished.");
                log("Final ticks L = " + std::to_string(left_encoder) +
                    " R = " + std::to_string(right_encoder) + "\n");
                break;
            }
        }

        delay(10);
    }
}

//===============================[ CONTROL ]====================================
/**
 * turn @angle degrees in counterclockwise direction
 */
void Ratatoskr::turn(int angle) {
    log("turning");
    float threshold = 0.5f;
    unsigned long t_now = micros();
    unsigned long t_last = t_now;  // avoid huge first dt
    // baseline angle in degrees
    float current_angle = m_gyro.getAngle(t_now, t_last);
    float target = current_angle + (float)angle;

    while (current_angle < target - threshold) {
        if (angle > 0) {
            m_motor_left.spin_cw(TURN_PWM);
            m_motor_right.spin_cw(TURN_PWM);
        } else {
            m_motor_left.spin_ccw(TURN_PWM);
            m_motor_right.spin_ccw(TURN_PWM);
        }
        t_now = micros();
        current_angle = m_gyro.getAngle(t_now, t_last);
        t_last = t_now;
    }

    stop();
}

bool Ratatoskr::too_close_front(uint16_t fl, uint16_t fr) {
    return (fl != 0) && (fr != 0) &&
           ((fl < STOP_DISTANCE && fr < STOP_DISTANCE + 20) ||
            (fl < STOP_DISTANCE + 20 && fr < STOP_DISTANCE));
}

/**
 * move @distance mm forwards with PID control
 */
void Ratatoskr::moveForward(int distance) {
    long target_counts = (long)(distance * ENCODER_COUNTS_PER_MM);
    int base_pwm = 200;

    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder = 0;
    long right_encoder = 0;
    long left_encoder_prev = 0;
    long right_encoder_prev = 0;

    uint16_t left_tof = 0;
    uint16_t right_tof = 0;

    float pwm_left = base_pwm;
    float pwm_right = base_pwm;

    const int loop_delay = 20;
    double time_step = 0.2;

    int t_now = millis();
    int t_prev = t_now;

    // TODO: This is also not very clean like this
    PID pid_encoders(time_step, 0.75, 0.8, 0.1, 50, 240);
    PID pid_distance(time_step, 0.25, 0.1, 0.15, 50, 240);

    while (!too_close_front(m_tof_front_left.get_reading(),
                            m_tof_front_right.get_reading()) &&
           (left_encoder + right_encoder) / 2 < target_counts) {
        left_encoder_prev = left_encoder;
        right_encoder_prev = right_encoder;
        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        int left_encoder_diff = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        left_tof = constrain(m_tof_left.get_reading(), 0, 80);
        right_tof = constrain(m_tof_right.get_reading(), 0, 80);

        // Prepare errors
        float tof_error = 0 - (left_tof - right_tof);
        float encoder_error = 0 - (left_encoder_diff - right_encoder_diff);

        t_now = millis();
        double t_diff = t_now - t_prev;
        t_prev = t_now;

        // Update PID controllers
        pid_encoders.TIME_STEP = t_diff / 100.0;
        pid_distance.TIME_STEP = t_diff / 100.0;
        float encoder_correction = pid_encoders.update(encoder_error);
        float tof_correction = pid_distance.update(tof_error);

        // Calculate new PWM
        pwm_left = base_pwm + .7 * tof_correction + .3 * encoder_correction;
        pwm_right = base_pwm - .7 * tof_correction - .3 * encoder_correction;

        pwm_left = constrain(pwm_left, 70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        // Apply motor commands
        m_motor_left.spin_cw(pwm_left);
        m_motor_right.spin_ccw(pwm_right);

        delay(loop_delay);
    }
    stop();
}

/**
 * SLAM THE BRAKES!
 */
void Ratatoskr::stop() {
    log("stop");
    m_motor_left.brake();
    m_motor_right.brake();
}

//===============================[ SENSING ]====================================
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

void Ratatoskr::update_visuals(Maze &maze) {}

bool Ratatoskr::wasReset() { return false; }
void Ratatoskr::ackReset() {}

void Ratatoskr::log(std::string msg) { Loggable::log(msg); }
