#include "ratatoskr.hpp"

#include "config.hpp"

using namespace Config;

Ratatoskr::Ratatoskr(GearMotor &motor_left, GearMotor &motor_right,
                     ToF &tof_left, ToF &tof_front_left, ToF &tof_front_right,
                     ToF &tof_right, MPU6050 &gyro, OLED &oled)
    : Mouse(),
      m_motor_left(motor_left),
      m_motor_right(motor_right),
      m_tof_left(tof_left),
      m_tof_front_left(tof_front_left),
      m_tof_front_right(tof_front_right),
      m_tof_right(tof_right),
      m_gyro(gyro),
      m_oled(oled),
      in_diagonal(false) {}

/* ============================[ MOVEMENT ]================================== */

constexpr float TRESH = 10.0f;
void Ratatoskr::moveDiagonal(float distance) {
    /*  Convert cells to mm and then to encoder counts */
    int distance_mm = distance * CELL_SIZE_MM * sqrt(2);
    long target_counts = (long)(distance_mm * ENCODER_COUNTS_PER_MM);

    const int BASE_PWM = FORWARD_PWM;

    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder = 0;
    long right_encoder = 0;
    long left_encoder_prev = 0;
    long right_encoder_prev = 0;

    bool is_end = false;
    bool nearing_end = false;

    float pwm_left = BASE_PWM;
    float pwm_right = BASE_PWM;

    const int loop_delay_ms = 20;

    int t_now = millis();
    int t_prev = t_now;

    long avg_counts = (left_encoder + right_encoder) / 2;
    m_gyro.update();

    while (avg_counts < target_counts) {
        m_gyro.update();
        m_gyro.get_next_angle();
        /*  ------------------ FRONT STOP ------------------ */
        uint16_t fl = m_tof_front_left.get_reading();
        uint16_t fr = m_tof_front_right.get_reading();
        if (too_close_front(fl, fr)) break;

        /*  ------------------ ENCODER PROGRESS ------------------ */
        int left_encoder_diff = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        float left_dist;
        float right_dist;

        is_end = fabs(left_dist - right_dist) < DIST_BETWEEN_SENSORS_MM - TRESH;
        nearing_end = fabs(target_counts - avg_counts) <
                      ENCODER_COUNTS_PER_MM * CELL_SIZE_MM * std::sqrt(2) / 2;

        if (nearing_end) {
            left_dist = constrain(fl, 0, MAX_DIST_MM - 50);
            right_dist = constrain(fr, 0, MAX_DIST_MM - 50);
        } else {
            left_dist = constrain(fl, 0, MAX_DIST_MM);
            right_dist = constrain(fr, 0, MAX_DIST_MM);
        }

        /*  ------------------ ERRORS ------------------ */
        float encoder_error = 0.0f - (left_encoder_diff - right_encoder_diff);
        float tof_error = 0.0f - (left_dist - right_dist);

        /*  ------------------ TIME STEP ------------------ */
        t_now = millis();
        float t_diff = (t_now - t_prev) / 100.0f;
        t_prev = t_now;

        /*  ------------------ PID UPDATES ------------------ */
        float encoder_correction = m_pid_encoders.update(t_diff, encoder_error);
        float tof_correction =
            m_pid_tof_front_diagonals.update(t_diff, tof_error);

        if (!is_end && !nearing_end) {
            pwm_left = BASE_PWM + PWM_UPDATE_RATIO * tof_correction +
                       (1 - PWM_UPDATE_RATIO) * encoder_correction;
            pwm_right = BASE_PWM - PWM_UPDATE_RATIO * tof_correction -
                        (1 - PWM_UPDATE_RATIO) * encoder_correction;
        } else {
            /*  No wall -> ToF PID output forced to 0 */
            tof_correction = 0.0f;
            pwm_left = BASE_PWM + encoder_correction;
            pwm_right = BASE_PWM - encoder_correction;
        }

        /*  ------------------ PWM COMPUTATION ------------------ */
        /*  Encoders are always active; ToF only when a wall exists */

        pwm_left = constrain(pwm_left, 70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        m_motor_left.spin_cw(pwm_left);
        m_motor_right.spin_ccw(pwm_right);

        delay(loop_delay_ms);

        /*  ------------------ ENCODER PROGRESS ------------------ */
        left_encoder_prev = left_encoder;
        right_encoder_prev = right_encoder;

        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        avg_counts = (left_encoder + right_encoder) / 2;
    }
    safe_stop();
    // delay(1000);
    // moveStraightMM(35.0f);
    delay(1000);
}

void Ratatoskr::turn(int angle) {
    safe_stop();
    delay(100);
    /* Verifies in which way is better to do the turn */
    if (abs(angle) == 180) {
        uint16_t distance_left = m_tof_left.get_reading();
        uint16_t distance_right = m_tof_right.get_reading();

        if (distance_left < distance_right) {
            angle = -angle;
        }
    }
    int requested_turn = angle;

    unsigned long t_start = millis();
    m_gyro.update();

    /*  Read current heading and define absolute target */
    float start_angle = m_gyro.get_current_angle();
    float target = start_angle + angle;

    /*  Initial turn speed */
    int turn_speed = MIN_TURN_PWM;
    if (angle < 60 && angle > -60) {
        turn_speed -= 4;
    }

    // turn_speed = constrain(turn_speed, MIN_TURN_PWM, MAX_TURN_PWM);

    while (millis() - t_start < TURN_TIME_LIMIT * (abs(angle) / 180.0f)) {
        m_gyro.update();
        float yaw = m_gyro.get_next_angle();
        update_screen(yaw, angle < 0 ? LOOK_RIGHT : LOOK_LEFT);

        float err = target - yaw;
        float abs_err = fabs(err);

        if (abs_err <= TURN_TRESHOLD) {
            /* Inside band: stop and shrink speed so next corrections are softer
             */
            coast();
            delay(1);
            if (turn_speed > MIN_TURN_PWM) {
                turn_speed -= 3;
                if (turn_speed < MIN_TURN_PWM) turn_speed = MIN_TURN_PWM;
            }
        } else {
            /*  Outside band: correct direction based on sign of error */
            // int pwm = constrain(turn_speed, MIN_TURN_PWM, MAX_TURN_PWM);

            if (err > 0) { /*  need to increase angle (CCW) */
                m_motor_left.spin_ccw(turn_speed);
                m_motor_right.spin_ccw(turn_speed);
            } else { /*  err < 0, need to decrease angle (CW) */
                m_motor_left.spin_cw(turn_speed);
                m_motor_right.spin_cw(turn_speed);
            }
        }
    }
    safe_stop();
    /*  Reset PIDs when turning */
    if (abs(requested_turn) == 180) {
        moveStraightMM(-35.0f);
    }
    m_pid_encoders.reset();
    m_pid_tof_sides.reset();
}

bool Ratatoskr::too_close_front(uint16_t fl, uint16_t fr) {
    bool res = (fl < STOP_DISTANCE && fr < STOP_DISTANCE);
    return res;
}

void Ratatoskr::moveForward(int distance_cells) {
    /*  Keep maze state in sync */
    Mouse::moveForward(distance_cells);
    Direction d = Mouse::getDirection();
    if (d == NORTH || d == EAST || d == SOUTH || d == WEST)
        moveForward((float)distance_cells);
    else
        moveDiagonal((float)distance_cells);
}

void Ratatoskr::moveForwardHalf(int num_half_steps) {
    safe_stop();
    Direction d = Mouse::getDirection();
    bool is_cardinal = (d == NORTH || d == EAST || d == SOUTH || d == WEST);
    if (!is_cardinal) {
        // in_diagonal = true;
        moveDiagonal((float)num_half_steps * 0.5f);
        return;
    }
    if (in_diagonal) {
        moveForward((float)num_half_steps * 0.5f);
        in_diagonal = false;
        return;
    }
    float JITTER_DISTANCE = 15.f;
    bool has_passed_pole = false;
    int changes = wallLeft() && wallRight() ? 1 : 2;
    int count = 0;
    if (!wallLeft()) {
        while (!has_passed_pole) {
            moveStraightMM(JITTER_DISTANCE);
            if (wallLeft() && count == 0) {
                count++;
            } else if ((!wallRight() || !wallLeft()) && count == 1) {
                has_passed_pole = true;
            }
        }
    } else if (!wallRight()) {
        while (!has_passed_pole) {
            moveStraightMM(JITTER_DISTANCE);
            if (wallRight() && count == 0) {
                count++;
            } else if ((!wallRight() || !wallLeft()) && count == 1) {
                has_passed_pole = true;
            }
        }
    } else if (changes == 1) {
        while (!has_passed_pole) {
            moveStraightMM(JITTER_DISTANCE);
            if ((!wallRight() || !wallLeft())) {
                has_passed_pole = true;
            }
        }
    }
    moveStraightMM(10.0f);
    delay(100);
    in_diagonal = true;
    has_passed_pole = false;  // i know we do this on init but fuck you
}

void Ratatoskr::moveForward(float distance_cells) {
    /*  Convert cells to mm */
    log("forward " + std::to_string(distance_cells));
    int distance_mm = distance_cells * CELL_SIZE_MM;

    /*  Overshoot factor grows with requested distance.
     *  For 1 cell: factor = 1.0
     *  For N cells: factor = 1 + FORWARD_OVERSHOOT_SLOPE * (N - 1)
     */
    float overshoot_factor = 1.0f;
    if (distance_cells >= 1.0f) {
        overshoot_factor += FORWARD_OVERSHOOT_SLOPE * (distance_cells - 1.0f);
    } else {
        // undershoot for half-cells
        overshoot_factor = 0.9f;
    }

    /*  Convert to encoder counts, including overshoot */
    long target_counts =
        (long)(distance_mm * ENCODER_COUNTS_PER_MM * overshoot_factor);

    int BASE_PWM = FORWARD_PWM + FORWARD_FAST_PWM_CHUNK * (distance_cells - 1);
    if (BASE_PWM > FORWARD_FAST_PWM_MAX) BASE_PWM = FORWARD_FAST_PWM_MAX;

    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    long left_encoder = 0;
    long right_encoder = 0;
    long left_encoder_prev = 0;
    long right_encoder_prev = 0;

    float pwm_left = BASE_PWM;
    float pwm_right = BASE_PWM;

    const int loop_delay_ms = 20;

    int t_now = millis();
    int t_prev = t_now;
    m_gyro.update();

    /*  Track whether we had any usable side wall on previous loop */
    bool had_side_wall_prev = false;

    long avg_counts = (left_encoder + right_encoder) / 2;
    while (avg_counts < target_counts) {
        // Go slow for first cell to correct bad turns
        // Only reduce speed when we're close to the target
        if ((avg_counts > target_counts * (distance_cells - 2) / distance_cells) ||
            avg_counts < target_counts * 1 / distance_cells)
            BASE_PWM = FORWARD_PWM;
        else {
            BASE_PWM =
                FORWARD_PWM + FORWARD_FAST_PWM_CHUNK * (distance_cells - 1);
            if (BASE_PWM > FORWARD_FAST_PWM_MAX)
                BASE_PWM = FORWARD_FAST_PWM_MAX;
        }

        /*  ------------------ FRONT STOP ------------------ */
        uint16_t fl = m_tof_front_left.get_reading();
        uint16_t fr = m_tof_front_right.get_reading();
        if (too_close_front(fl, fr)) break;

        /*  ------------------ ENCODER PROGRESS ------------------ */
        int left_encoder_diff = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        /*  ------------------ SIDE ToF READING ------------------ */
        uint16_t left_raw = m_tof_left.get_reading();
        uint16_t right_raw = m_tof_right.get_reading();

        bool left_ok = (left_raw > 0 && left_raw < MAX_TOF_VALID_MM);
        bool right_ok = (right_raw > 0 && right_raw < MAX_TOF_VALID_MM);

        bool has_side_wall = left_ok || right_ok;

        /*  If we just lost all side walls, reset the side PID */
        if (!has_side_wall && had_side_wall_prev) {
            m_pid_tof_sides.reset();
        }

        /*  ------------------ ERRORS ------------------ */
        float encoder_error = 0.0f - (left_encoder_diff - right_encoder_diff);
        float tof_error = 0.0f;

        if (has_side_wall) {
            if (left_ok && right_ok) {
                /*  Corridor: center between walls */
                tof_error = (float)right_raw - (float)left_raw;
            } else if (right_ok && !left_ok) {
                /*  Only right wall: keep right distance at TOF_SIDE_EXPECTED_MM
                 */
                tof_error = (float)right_raw - (float)TOF_SIDE_EXPECTED_MM;
            } else if (left_ok && !right_ok) {
                /*  Only left wall: keep left distance at TOF_SIDE_EXPECTED_MM
                 */
                tof_error = (float)TOF_SIDE_EXPECTED_MM - (float)left_raw;
            }
        } else {
            /*  No usable side walls -> no side correction */
            tof_error = 0.0f;
        }

        /*  ------------------ TIME STEP ------------------ */
        t_now = millis();
        float t_diff = (t_now - t_prev) / 100.0f;

        m_gyro.update();
        float gyro_angle = m_gyro.get_next_angle();
        update_screen(gyro_angle);
        t_prev = t_now;

        /*  ------------------ PID UPDATES ------------------ */
        float encoder_correction = m_pid_encoders.update(t_diff, encoder_error);
        float tof_correction = 0.0f;

        if (has_side_wall) {
            tof_correction = m_pid_tof_sides.update(t_diff, tof_error);

            /*  Clamp side correction so it can't yank PWM too hard */
            tof_correction = constrain(tof_correction, -MAX_PWM_CORRECTION,
                                       +MAX_PWM_CORRECTION);
            pwm_left = BASE_PWM + PWM_UPDATE_RATIO * tof_correction +
                       (1 - PWM_UPDATE_RATIO) * encoder_correction;
            pwm_right = BASE_PWM - PWM_UPDATE_RATIO * tof_correction -
                        (1 - PWM_UPDATE_RATIO) * encoder_correction;
        } else {
            /*  No wall: sides PID output forced to 0 */
            tof_correction = 0.0f;
            pwm_left = BASE_PWM + encoder_correction;
            pwm_right = BASE_PWM - encoder_correction;
        }

        /*  ------------------ PWM COMPUTATION ------------------ */
        pwm_left = constrain(pwm_left, 70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        m_motor_left.spin_cw(pwm_left);
        m_motor_right.spin_ccw(pwm_right);

        had_side_wall_prev = has_side_wall;

        delay(loop_delay_ms);

        /*  ------------------ ENCODER PROGRESS ------------------ */
        left_encoder_prev = left_encoder;
        right_encoder_prev = right_encoder;

        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        avg_counts = (left_encoder + right_encoder) / 2;
    }
    safe_stop();
}
void Ratatoskr::moveStraightMM(float mm) {
    /* Distance in encoder counts (always positive) */
    long target_counts = (long)(fabs(mm) * ENCODER_COUNTS_PER_MM);

    /* Reset encoders so we measure this move cleanly */
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();
    m_pid_encoders.reset();

    long left_encoder = 0;
    long right_encoder = 0;
    long left_encoder_prev = 0;
    long right_encoder_prev = 0;

    /* Direction: +1 = forward, -1 = backward */
    int dir = (mm >= 0.0f) ? 1 : -1;

    const int loop_delay = 10; /*  shorter loop, it's a tiny move */

    int t_now = millis();
    int t_prev = t_now;
    int BASE_PWM = FORWARD_PWM - 15;

    /* Same encoder PID as moveForward */
    long avg_counts = (labs(left_encoder) + labs(right_encoder)) / 2;

    while (avg_counts < target_counts) {
        int left_encoder_diff = left_encoder - left_encoder_prev;
        int right_encoder_diff = right_encoder - right_encoder_prev;

        /* Time step */
        t_now = millis();
        float t_diff = (t_now - t_prev) / 100.0f;
        t_prev = t_now;
        /*  ------------------ FRONT STOP ------------------ */
        uint16_t fl = m_tof_front_left.get_reading();
        uint16_t fr = m_tof_front_right.get_reading();
        if (too_close_front(fl, fr)) break;

        /* Same sign convention as moveForward */
        float encoder_error = 0.0f - (left_encoder_diff - right_encoder_diff);
        float encoder_correction = m_pid_encoders.update(t_diff, encoder_error);

        /* Basic PWM with encoder correction */
        float pwm_left = BASE_PWM + (1 - PWM_UPDATE_RATIO) * encoder_correction;
        float pwm_right =
            BASE_PWM - (1 - PWM_UPDATE_RATIO) * encoder_correction;

        pwm_left = constrain(pwm_left, 70, 240);
        pwm_right = constrain(pwm_right, 70, 240);

        if (dir > 0) {
            /* forward */
            m_motor_left.spin_cw(pwm_left);
            m_motor_right.spin_ccw(pwm_right);
        } else {
            /* backward */
            m_motor_left.spin_ccw(pwm_left);
            m_motor_right.spin_cw(pwm_right);
        }

        delay(loop_delay);
        /*  Read encoders */
        left_encoder_prev = left_encoder;
        right_encoder_prev = right_encoder;

        left_encoder = m_motor_left.get_encoder_count();
        right_encoder = m_motor_right.get_encoder_count();

        avg_counts = (labs(left_encoder) + labs(right_encoder)) / 2;
    }
    safe_stop();
}

/**
 * STOOOOOOP
 */
void Ratatoskr::stop() {
    m_motor_left.brake();
    m_motor_right.brake();
}

void Ratatoskr::coast() {
    m_motor_left.coast();
    m_motor_right.coast();
}

void Ratatoskr::safe_stop() {
    coast();
    delay(1);
    stop();
    delay(2);
    coast();
}

/* =============================[ SENSING ] ================================= */
bool Ratatoskr::wallFront() {
    uint16_t distance_front_left = m_tof_front_left.get_reading();
    uint16_t distance_front_right = m_tof_front_right.get_reading();

    return (distance_front_left > 0) && (distance_front_left < FRONT_WALL_MM) ||
           (distance_front_right > 0) && (distance_front_right < FRONT_WALL_MM);
}

bool Ratatoskr::wallRight() {
    uint16_t distance_right = m_tof_right.get_reading();
    return (distance_right > 0) && (distance_right < SIDE_WALL_MM);
}

bool Ratatoskr::wallLeft() {
    uint16_t distance_left = m_tof_left.get_reading();
    return (distance_left > 0) && (distance_left < SIDE_WALL_MM);
}

void Ratatoskr::update_screen(float gyro_angle, Face face) {
    m_oled.clear();
    switch (m_oled.mode) {
        case DEBUG: {
            uint16_t left_rpm = m_motor_left.get_rpm();
            uint16_t right_rpm = m_motor_right.get_rpm();
            m_oled.update_status_bar(abs(floor(gyro_angle)), left_rpm,
                                     right_rpm);

            uint16_t tof_left = m_tof_left.get_reading();
            uint16_t tof_front_left = m_tof_front_left.get_reading();
            uint16_t tof_front_right = m_tof_front_right.get_reading();
            uint16_t tof_right = m_tof_right.get_reading();
            m_oled.update_ToFs(tof_left, tof_front_left, tof_front_right,
                               tof_right);

            break;
        }

        default:
            m_oled.draw_face(face);
    }
    m_oled.display();
}

void Ratatoskr::update_visuals(Maze &maze) {}
bool Ratatoskr::wasReset() { return false; }
void Ratatoskr::ackReset() {}

void Ratatoskr::log(std::string msg) { ESPLogger::log(msg); }
