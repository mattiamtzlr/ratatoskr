#include "ratatoskr.hpp"

#include "pid.hpp"
#include "pins.hpp"
#define PERIOD 868.75
#define TIME_PER_CELL 250.0

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

    Serial.println("=== Encoder calibration mode ===");
    Serial.println(
        "Move the mouse manually. Press Enter in Serial Monitor when done.\n");

    unsigned long last_print = 0;
    while (true) {
        long left_encoder = m_motor_left.get_encoder_count();
        long right_encoder = m_motor_right.get_encoder_count();

        unsigned long now = millis();
        if (now - last_print >= 100) {
            last_print = now;
            Serial.print("Ticks L=");
            Serial.print(left_encoder);
            Serial.print(" R=");
            Serial.println(right_encoder);
        }

        if (Serial.available() > 0) {
            int c = Serial.read();
            if (c == '\n' || c == '\r') {
                Serial.println("\nCalibration finished.");
                Serial.print("Final ticks L=");
                Serial.print(left_encoder);
                Serial.print(" R=");
                Serial.println(right_encoder);
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
 * move @distance [one cell (16 cm)] forwards with PID control
 */
void Ratatoskr::moveForward(int distance) {
    Mouse::moveForward(distance);  // Update position in maze

    // Calculate target encoder counts for desired distance
    // long target_counts = (long)(distance * MM_PER_CELL *
    // ENCODER_COUNTS_PER_MM); // delete MM_PER_CELL if distance is in mm
    long target_counts =
        (long)(distance * ENCODER_COUNTS_PER_MM);  // i want distance in mm

    // Reset encoder counts at start
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    // PID controller instance
    PID pid;

    // Control loop parameters
    const float time_step = 0.02;  // 20ms update rate
    const int loop_delay = 20;
    long left_encoder = 0;
    long right_encoder = 0;
    std::pair<int, int> corrected_pwm = {};

    // Main control loop - run until target distance reached
    while ((left_encoder + right_encoder) / 2 < target_counts) {
        // Read current encoder values
        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        // Update PID controller
        corrected_pwm =
            pid.update(time_step, m_tof_left.read(), m_tof_right.read(),
                       left_encoder, right_encoder);

        // Apply motor commands
        m_motor_left.spin_ccw(corrected_pwm.first);
        m_motor_right.spin_cw(corrected_pwm.second);

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
    m_motor_left.brake();
    m_motor_right.brake();
    delay(100);
    m_motor_left.coast();
    m_motor_right.coast();
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
