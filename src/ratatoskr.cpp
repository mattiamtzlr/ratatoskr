#include "ratatoskr.hpp"

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

constexpr int MIN_TURN_PWM = 190;
constexpr int MAX_TURN_PWM = 200;
constexpr int TURN_TIME_LIMIT = 5000;

//===============================[ CONTROL ]====================================
/**
 * turn @angle degrees in counterclockwise direction
 */
void Ratatoskr::turn(int angle) {
    float threshold = 1.0f; 
    PID pid_turn(0.5f, 0.05f, 0.0f);  // Added small I term to eliminate steady-state error

    unsigned long t_start = millis();
    unsigned long t_now = micros();
    unsigned long t_last = t_now;

    float current_angle = m_gyro.getAngle(t_now, t_last);
    float target = current_angle + angle;
    float error = angle;
    int pwm = (MIN_TURN_PWM) * 90/angle;
    pwm = constrain(pwm, MIN_TURN_PWM - 7, MAX_TURN_PWM); // what min is here is the 180 speed
    // the goal is to have higher min speed for smaller angles so we have better momentum
    // and slower for large so we dont overshoot too much (this doesnt even use PID lmao, just vibe stop using gyro)


    while ((millis() - t_start) < TURN_TIME_LIMIT) {
        error = target - current_angle;
        
        // Actually use the threshold to exit early
        if (abs(error) < threshold) {
            break;
        }

        t_now = micros();
        float t_diff = ((float)(t_now - t_last)) / 1e6f;
        int angle_correction = pid_turn.update(t_diff, error);
        
        // Only apply motor commands if correction is meaningful
        if (abs(angle_correction) > 10) {  // Adjust this threshold
            // int pwm = constrain(abs(angle_correction), MIN_TURN_PWM, MAX_TURN_PWM); lmao L PID bro (jk lets try it later)
            
            if (angle_correction > 0) { /* turning CCW */
                m_motor_left.spin_ccw(pwm);
                m_motor_right.spin_ccw(pwm);
            } else { /* turning CW */
                m_motor_left.spin_cw(pwm);
                m_motor_right.spin_cw(pwm);
            }
        } else {
            // Stop motors when correction is too small
            m_motor_left.coast();  // or brake() for faster settling idk
            m_motor_right.coast();
        }

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
    const int BASE_PWM = 200;

    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder = 0;
    long right_encoder = 0;
    long left_encoder_prev = 0;
    long right_encoder_prev = 0;

    uint16_t left_tof = 0;
    uint16_t right_tof = 0;

    float pwm_left = BASE_PWM;
    float pwm_right = BASE_PWM;

    const int loop_delay = 20;

    int t_now = millis();
    int t_prev = t_now;

    // TODO: This is also not very clean like this
    PID pid_encoders(0.75, 0.8, 0.1);
    PID pid_distance(0.25, 0.1, 0.15);

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
        float t_diff = (t_now - t_prev) / 100.0;
        t_prev = t_now;

        // Update PID controllers
        float encoder_correction = pid_encoders.update(t_diff, encoder_error);
        float tof_correction = pid_distance.update(t_diff, tof_error);

        // Calculate new PWM
        pwm_left = BASE_PWM + .7 * tof_correction + .3 * encoder_correction;
        pwm_right = BASE_PWM - .7 * tof_correction - .3 * encoder_correction;

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

void Ratatoskr::log(std::string msg) { ESPLogger::log(msg); }
