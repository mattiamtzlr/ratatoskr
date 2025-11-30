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


constexpr int TURN_TIME_LIMIT = 2000;
constexpr float TURN_TRESHOLD = 0.2f;

//===============================[ CONTROL ]====================================
/**
 * turn @angle degrees in counterclockwise direction
 */
void Ratatoskr::turn(int angle) {
    moveStraightMM(19);
    int requested_turn = angle;
    // Reset PIDs when turning
    m_pid_encoders.reset();
    m_pid_distance.reset();

    // Adjust angle so that final heading is snapped to a cardinal direction
    // angle = snapToAngle(angle);

    unsigned long t_start = millis();
    unsigned long t_now   = micros();
    unsigned long t_last  = t_now;

    // Read current heading and define absolute target
    float start_angle = m_gyro.getAngle(t_now, t_last);
    float target      = start_angle + angle;

    // Initial turn speed
    int turn_speed = ((MIN_TURN_PWM + MAX_TURN_PWM) / 2) * 90 / abs(angle);
    turn_speed     = constrain(turn_speed, MIN_TURN_PWM, MAX_TURN_PWM);

    while (millis() - t_start < TURN_TIME_LIMIT * (abs(angle) / 180.0f)) {
        t_now = micros();
        float yaw = m_gyro.getAngle(t_now, t_last);
        t_last = t_now;

        float err     = target - yaw;
        float abs_err = fabs(err);

        if (abs_err <= TURN_TRESHOLD) {
            // Inside band: stop and shrink speed so next corrections are softer
            coast();
            if (turn_speed > MIN_TURN_PWM) {
                turn_speed -= 3;
                if (turn_speed < MIN_TURN_PWM)
                    turn_speed = MIN_TURN_PWM;
            }
        } else {
            // Outside band: correct direction based on sign of error
            int pwm = constrain(turn_speed, MIN_TURN_PWM, MAX_TURN_PWM);

            if (err > 0) {  // need to increase angle (CCW)
                m_motor_left.spin_ccw(pwm);
                m_motor_right.spin_ccw(pwm);
            } else {        // err < 0, need to decrease angle (CW)
                m_motor_left.spin_cw(pwm);
                m_motor_right.spin_cw(pwm);
            }
        }
    }

    coast(); // reduce overshoot a bit
    delay(1);
    stop();

    moveStraightMM(-10);
    coast();
}
int Ratatoskr::snapToAngle(int target) {
    // Current absolute angle from gyro (can be any value, not necessarily 0–360)
    float current_angle = m_gyro.getAngle();

    // Find nearest multiple of 90° to the current angle
    int k;
    if (current_angle >= 0.0f) {
        k = static_cast<int>((current_angle + 45.0f) / 90.0f);
    } else {
        k = static_cast<int>((current_angle - 45.0f) / 90.0f);
    }
    int nearest_cardinal = k * 90;  // e.g. ..., -180, -90, 0, 90, 180, ...

    // Final desired cardinal heading after the requested grid turn
    int target_cardinal = nearest_cardinal + target;

    // Relative angle we actually need to turn from the *real* current angle
    int relative_turn = target_cardinal - static_cast<int>(current_angle);

    return relative_turn;
}


bool Ratatoskr::too_close_front(uint16_t fl, uint16_t fr) {
    bool res = (fl != 0) && (fr != 0) &&
          (fl < STOP_DISTANCE ||
           fr < STOP_DISTANCE);
    // log("Too close?" + res ? " YES" : " NO");
    return res;
}

// lord forgive me for what I'm about to do (constexpr in cpp)
constexpr float MAX_PWM_CORRECTION = 30.0f;

/**
 * move @distance cells forward with PID control
 */
// Side ToF geometry:
// - Cell: 16 cm -> half-cell = 8 cm = 80 mm
// - Sensor offset from center: 30 mm
// => ideal sensor–wall distance when centered = 80 - 30 = 50 mm
constexpr uint16_t TARGET_SIDE_MM   = 50;   // desired side distance at sensor
constexpr uint16_t MAX_TOF_VALID_MM = 120;  // beyond this: treat as "no wall"

/**
 * move @distance cells forward with PID control
 */
void Ratatoskr::moveForward(int distance_cells) {
    // Keep maze state in sync
    Mouse::moveForward(distance_cells);

    // Soft reset of previous motor commands
    coast();
    delay(5);
    stop();
    delay(5);
    coast();

    // Convert cells to mm and then to encoder counts
    int distance_mm    = distance_cells * CELL_SIZE_MM;
    long target_counts = (long)(distance_mm * ENCODER_COUNTS_PER_MM);

    const int BASE_PWM = FORWARD_PWM;

    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder       = 0;
    long right_encoder      = 0;
    long left_encoder_prev  = 0;
    long right_encoder_prev = 0;

    float pwm_left  = BASE_PWM;
    float pwm_right = BASE_PWM;

    const int loop_delay_ms = 20;

    int t_now  = millis();
    int t_prev = t_now;

    // Track whether we had any usable side wall on previous loop
    bool had_side_wall_prev = false;

    while (true) {
        // ------------------ FRONT STOP (optional) ------------------
        // uint16_t fl = m_tof_front_left.get_reading();
        // uint16_t fr = m_tof_front_right.get_reading();
        // if (too_close_front(fl, fr)) {
        //     break;
        // }

        // ------------------ ENCODER PROGRESS ------------------
        left_encoder_prev  = left_encoder;
        right_encoder_prev = right_encoder;

        left_encoder  = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        long avg_counts = (left_encoder + right_encoder) / 2;

        // Reached requested distance
        if (avg_counts >= target_counts) {
            break;
        }

        int left_encoder_diff  = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        // ------------------ SIDE ToF READING ------------------
        uint16_t left_raw  = m_tof_left.get_reading();
        uint16_t right_raw = m_tof_right.get_reading();

        bool left_ok  = (left_raw  > 0 && left_raw  < MAX_TOF_VALID_MM);
        bool right_ok = (right_raw > 0 && right_raw < MAX_TOF_VALID_MM);

        bool has_side_wall = left_ok && right_ok;

        // If we just lost all side walls, reset the distance PID
        if (!has_side_wall && had_side_wall_prev) {
            m_pid_distance.reset();
        }

        // ------------------ ERRORS ------------------
        float encoder_error = 0.0f - (left_encoder_diff - right_encoder_diff);
        float tof_error     = 0.0f;

        if (has_side_wall) {
            if (left_ok && right_ok) {
                // Corridor: center between walls
                tof_error = (float)right_raw - (float)left_raw;
            } else if (right_ok && !left_ok) {
                // Only right wall: keep right distance at TARGET_SIDE_MM
                tof_error = (float)right_raw - (float)TARGET_SIDE_MM;
            } else if (left_ok && !right_ok) {
                // Only left wall: keep left distance at TARGET_SIDE_MM
                tof_error = (float)left_raw - (float)TARGET_SIDE_MM;
            }
        } else {
            // No usable side walls -> no side correction
            tof_error = 0.0f;
        }

        // ------------------ TIME STEP ------------------
        t_now = millis();
        float t_diff = (t_now - t_prev) / 100.0f; // same time scaling as before
        t_prev = t_now;

        // ------------------ PID UPDATES ------------------
        float encoder_correction = m_pid_encoders.update(t_diff, encoder_error);

        float tof_correction = 0.0f;
        if (has_side_wall) {
            tof_correction = m_pid_distance.update(t_diff, tof_error);

            // Clamp side correction so it can't yank PWM too hard
            tof_correction = constrain(
                tof_correction,
                -MAX_PWM_CORRECTION,
                +MAX_PWM_CORRECTION
            );
        } else {
            // No wall: distance PID output forced to 0
            tof_correction = 0.0f;
        }

        // ------------------ PWM COMPUTATION ------------------
        // Encoders are always active; ToF only when a wall exists
        pwm_left  = BASE_PWM + 0.6f * tof_correction + 0.4f * encoder_correction;
        pwm_right = BASE_PWM - 0.6f * tof_correction - 0.4f * encoder_correction;

        pwm_left  = constrain(pwm_left,  70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        // Same direction convention as before (forward)
        m_motor_left.spin_cw(pwm_left);
        m_motor_right.spin_ccw(pwm_right);

        had_side_wall_prev = has_side_wall;

        delay(loop_delay_ms);
    }

    // Dark magic brake so that motors don't shit themselves
    coast();
    delay(5);
    stop();
    delay(20);
    coast();
}


void Ratatoskr::moveStraightMM(float mm) {
    // Distance in encoder counts (always positive)
    long target_counts = (long)(fabs(mm) * ENCODER_COUNTS_PER_MM);

    // Reset encoders so we measure this move cleanly
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder       = 0;
    long right_encoder      = 0;
    long left_encoder_prev  = 0;
    long right_encoder_prev = 0;

    // Direction: +1 = forward, -1 = backward
    int dir = (mm >= 0.0f) ? 1 : -1;

    const int loop_delay = 10;  // shorter loop, it's a tiny move

    int t_now  = millis();
    int t_prev = t_now;

    // Same encoder PID as moveForward
    PID pid_encoders(0.75, 0.8, 0.1);

    while (true) {
        // Read encoders
        left_encoder_prev  = left_encoder;
        right_encoder_prev = right_encoder;

        left_encoder  = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        // Progress based on absolute counts (works for forward/backward)
        long avg_counts = (labs(left_encoder) + labs(right_encoder)) / 2;
        if (avg_counts >= target_counts) {
            break;
        }

        int left_encoder_diff  = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        // Time step
        t_now = millis();
        float t_diff = (t_now - t_prev) / 100.0f;
        t_prev = t_now;

        // Encoder PID (same sign convention as moveForward)
        float encoder_error      = 0.0f - (left_encoder_diff - right_encoder_diff);
        float encoder_correction = pid_encoders.update(t_diff, encoder_error);

        // Basic PWM with encoder correction
        float pwm_left  = FORWARD_PWM + 20 + 0.4f * encoder_correction;
        float pwm_right = FORWARD_PWM + 20 - 0.4f * encoder_correction;

        pwm_left  = constrain(pwm_left,  70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        // Apply direction (forward/backward) but keep same differential
        if (dir > 0) {
            // Forward (same as your moveForward)
            m_motor_left.spin_cw(pwm_left);
            m_motor_right.spin_ccw(pwm_right);
        } else {
            // Backward: reverse motor directions
            m_motor_left.spin_ccw(pwm_left);
            m_motor_right.spin_cw(pwm_right);
        }

        delay(loop_delay);
    }

    coast();
    delay(2);
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

void Ratatoskr::coast() {
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

void Ratatoskr::log(std::string msg) { ESPLogger::log(msg); }
