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
            log("Ticks L = " + std::to_string(left_encoder) + " R = " + std::to_string(right_encoder));
        } 

        if (Serial.available() > 0) {
            int c = Serial.read();
            if (c == '\n' || c == '\r') {
                log("\nCalibration finished.");
                log("Final ticks L = " + std::to_string(left_encoder) + " R = " + std::to_string(right_encoder) + "\n");
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

/**
 * move @distance mm forwards with PID control
 */
void Ratatoskr::moveForward(int distance) {
    log("moving forward " + std::to_string(distance) + "mm");

    // Calculate target encoder counts for desired distance
    long target_counts = (long)(distance * ENCODER_COUNTS_PER_MM);

    // Reset encoder counts at start
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    // Control loop parameters
    const float time_step = 0.02;  // 20ms update rate
    const int loop_delay = 20;

    long left_encoder = 0;
    long right_encoder = 0;
    uint16_t left_tof = 0;
    uint16_t right_tof = 0;
    float target_distance = .0;
    float target_encoder = .0;

    // Base PWM that ramps up over time
    float base_pwm = 70.0;  // TODO: move to header
    const float accel_time =
        500.0;  // milliseconds to reach FORWARD_PWM  TODO: Move to header
    const float decel_distance =
        30.0;  // mm before target to start decelerating  TODO: Move to header
    const float pwm_increment =
        (FORWARD_PWM - 70.0) / (accel_time / loop_delay);
    const long decel_counts = (long)(decel_distance * ENCODER_COUNTS_PER_MM);

    // Initial pwm values
    float pwm_left = base_pwm;
    float pwm_right = base_pwm;

    // PID controller instances
    PID pid_distance(time_step, 0.25, 0.1, 0.00, 50,
                     240);  // TODO: This is also not very clean like this
    PID pid_encoders(time_step, 0.75, 0.8, 0.1, 50, 240);

    // Steps
    int steps = 0;

    // Main control loop - run until target distance reached
    while ((left_encoder + right_encoder) / 2 < target_counts) {
        // Read current values
        left_tof = constrain(m_tof_left.read(), 0, 80);
        right_tof = constrain(m_tof_right.read(), 0, 80);
        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        long avg_encoder = (left_encoder + right_encoder) / 2;
        long remaining_counts = target_counts - avg_encoder;

        // Log
        if (steps++ % 10 == 0) {
            log("ToF (l, r): " + std::to_string(left_tof) + ", " +
                std::to_string(right_tof) +
                " | Base PWM: " + std::to_string((int)base_pwm) +
                " | Remaining: " + std::to_string(remaining_counts));
        }

        // Ramp up base PWM over time, or ramp down near target
        // NOTE: It might make sense to have decel_counts be something we
        // calculate based on percentages. e. g. have of a move_forward(x) call
        // result in 10% speedup, 60% full speed and 30% slowdown
        if (remaining_counts < decel_counts) {
            // Deceleration phase - linearly decrease to 70
            float decel_ratio = (float)remaining_counts / (float)decel_counts;
            base_pwm = 70.0 + (FORWARD_PWM - 70.0) *
                                  decel_ratio;  // NOTE: magic number
        } else if (base_pwm < FORWARD_PWM) {
            // Acceleration phase
            base_pwm += pwm_increment;
            if (base_pwm > FORWARD_PWM) {
                base_pwm = FORWARD_PWM;
            }
        }

        // Clamp and adjust tof readings
        float left_dist = (left_tof == 0 && right_tof > 0) ? 0.75 : left_tof;
        float right_dist = (right_tof == 0 && left_tof > 0) ? 0.75 : right_tof;

        // Prepare errors
        float tof_error = target_distance - (left_dist - right_dist);
        float encoder_error = target_encoder - (left_encoder - right_encoder);

        // Update PID controllers
        float tof_correction = pid_distance.update(tof_error);
        float encoder_correction = pid_encoders.update(encoder_error);

        // Calculate new PWM
        pwm_left = base_pwm + tof_correction + encoder_correction;
        pwm_right = base_pwm - tof_correction - encoder_correction;

        pwm_left = constrain(pwm_left, 70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        // Apply motor commands
        m_motor_left.spin_ccw(pwm_left);
        m_motor_right.spin_cw(pwm_right);

        // Wait for next control cycle
        delay(loop_delay);
    }

    // Stop motors when target reached
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

void Ratatoskr::log(std::string msg) {
    Loggable::log(msg);
}
