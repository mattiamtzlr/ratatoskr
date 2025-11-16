#include "ratatoskr.hpp"
#include "pins.hpp"
#define PERIOD 868.75
#define TIME_PER_CELL 250.0

// PID Controller Class
class SystemPID {
   public:
    SystemPID()
        : distanceKp(0.25),
          distanceKi(0.1),
          distanceKd(0.00),
          encoderKp(0.75),
          encoderKi(0.0),
          encoderKd(0.0),
          distancePrevError(0),
          encoderPrevError(0),
          distanceIntegral(0),
          encoderIntegral(0),
          basePWM_forward(Ratatoskr::FORWARD_PWM) {}

    void update(float dt, float leftDistance, float rightDistance,
                float leftEncoder, float rightEncoder) {
        // Constants
        const float targetPosition = 0.0;

        leftDistance =
            (leftDistance == 0 && rightDistance > 0) ? 0.75 : leftDistance;

        rightDistance =
            (rightDistance == 0 && leftDistance > 0) ? 0.75 : rightDistance;

        // Lateral position error (positive = too far right, negative = too far
        // left)
        float leftConstrained = constrain(leftDistance, 0, 80);
        float rightConstrained = constrain(rightDistance, 0, 80);
        float currentPosition = leftConstrained - rightConstrained;

        // PID for lateral control
        float distanceError = targetPosition - currentPosition;
        distanceIntegral += distanceError * dt;
        float distanceDerivative = (distanceError - distancePrevError) / dt;
        distancePrevError = distanceError;

        float side_correction = distanceKp * distanceError +
                                distanceKi * distanceIntegral +
                                distanceKd * distanceDerivative;

        // PID for encoder synchronization (keep wheels matched) (think of plane
        // wings)
        float targetEncoder = 0.0;
        float currentEncoder = leftEncoder - rightEncoder;
        float encoderError = targetEncoder - currentEncoder;
        encoderIntegral += encoderError * dt;
        float encoderDerivative = (encoderError - encoderPrevError) / dt;

        float encoderCorrection = encoderKp * encoderError +
                                  encoderKi * encoderIntegral +
                                  encoderKd * encoderDerivative;
        encoderPrevError = encoderError;

        // Disable lateral correction if both walls are far or absent
        // if ((leftDistance >= 40 || leftDistance == 0) &&
        //     (rightDistance >= 40 || rightDistance == 0)) {
        //     side_correction = 0;
        // }

        // Calculate final PWM values
        corrected_left_PWM =
            basePWM_forward + side_correction + encoderCorrection;
        corrected_right_PWM =
            basePWM_forward - side_correction - encoderCorrection;
        Serial.println("corrections: side enc " + String(encoderCorrection));
        // Constrain to valid PWM range
        corrected_left_PWM = constrain(corrected_left_PWM, 40, 255);
        corrected_right_PWM = constrain(corrected_right_PWM, 40, 255);
    }

    void reset() {
        distancePrevError = 0;
        encoderPrevError = 0;
        distanceIntegral = 0;
        encoderIntegral = 0;
    }

    float getLeftPWM() { return corrected_left_PWM; }
    float getRightPWM() { return corrected_right_PWM; }

   private:
    // PID coefficients
    double distanceKp, distanceKi, distanceKd;
    double encoderKp, encoderKi, encoderKd;

    // PID state variables
    double distancePrevError, distanceIntegral;
    double encoderPrevError, encoderIntegral;

    double basePWM_forward;
    float corrected_left_PWM;
    float corrected_right_PWM;
};

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

//===============================[ CONTROL ]====================================
/**
 * turn @angle radians in counterclockwise direction
 */
void Ratatoskr::turn(int angle) {
    float threshold = 0.5f;
    unsigned long t_now = micros();
    unsigned long t_last = 0;
    float current_angle = m_gyro.getAngle(t_now, t_last);
    float target = current_angle + angle;
    t_last = t_now;

    if (angle > 0) {
        while (current_angle < target - threshold) {
            Serial.println(current_angle);
            m_motor_left.spin_cw(TURN_PWM);
            m_motor_right.spin_cw(TURN_PWM);
            t_now = micros();
            current_angle = m_gyro.getAngle(t_now, t_last);
            t_last = t_now;
        }
    } else {
        while (current_angle > target + threshold) {
            m_motor_left.spin_ccw(TURN_PWM);
            m_motor_right.spin_ccw(TURN_PWM);
            t_now = micros();
            current_angle = m_gyro.getAngle(t_now, t_last);
            t_last = t_now;
        }
    }
    stop();
}

/**
 * move @distance [one cell (16 cm)] forwards with PID control
 */
void Ratatoskr::moveForward(int distance) {
    Mouse::moveForward(distance);  // Update position in maze

    // Calculate target encoder counts for desired distance
    // Assuming encoder counts per cm - you'll need to calibrate this
    const float ENCODER_COUNTS_PER_CM = 10.0;  // TODO: Calibrate this value
    const float CM_PER_CELL = 16.0;
    long target_counts = (long)(distance * CM_PER_CELL * ENCODER_COUNTS_PER_CM);

    // Reset encoder counts at start
    m_motor_left.reset_encoder_count();
    m_motor_right.reset_encoder_count();

    // PID controller instance
    SystemPID pid;

    // Control loop parameters
    const float dt = 0.02;  // 20ms update rate
    const int loop_delay_ms = 20;

    m_motor_left.spin_ccw(50);  // INITIAL PWM
    m_motor_right.spin_cw(50);
    delay(loop_delay_ms);

    // Main control loop - run until target distance reached
    while (true) {
        Serial.println("encoders: L " +
                       String(m_motor_left.get_encoder_count()) + " R " +
                       String(m_motor_right.get_encoder_count()));
        // Read current encoder values
        long left_encoder = m_motor_left.get_encoder_count();
        long right_encoder = m_motor_right.get_encoder_count();

        // Check if we've reached the target distance
        long avg_encoder = (left_encoder + right_encoder) / 2;
        if (avg_encoder >= target_counts) {
            break;
        }

        // Read ToF sensor distances (in mm)
        uint16_t left_distance = m_tof_left.read();
        uint16_t right_distance = m_tof_right.read();

        // Update PID controller
        pid.update(dt, left_distance, right_distance, left_encoder,
                   right_encoder);

        // Get corrected PWM values
        float left_pwm = pid.getLeftPWM();
        float right_pwm = pid.getRightPWM();

        // Apply motor commands
        m_motor_left.spin_ccw((int)left_pwm);
        m_motor_right.spin_cw((int)right_pwm);

        // Wait for next control cycle
        delay(loop_delay_ms);
    }

    // Stop motors when target reached
    stop();
}

/**
 * SLAM THE BRAKES!
 */
void Ratatoskr::stop() {
    /*m_motor_left.stop();
    m_motor_right.stop();*/
    analogWrite(MOTOR_L_IN1, 255);
    analogWrite(MOTOR_L_IN2, 255);
    analogWrite(MOTOR_R_IN1, 255);
    analogWrite(MOTOR_R_IN2, 255);
    delay(100);
    analogWrite(MOTOR_L_IN1, 0);
    analogWrite(MOTOR_L_IN2, 0);
    analogWrite(MOTOR_R_IN1, 0);
    analogWrite(MOTOR_R_IN2, 0);
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
