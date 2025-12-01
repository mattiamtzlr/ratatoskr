#pragma once
#include <Arduino.h>
#include "ToF.hpp"
#include "gear_motor.hpp"
#include "gyro.hpp"
#include "led_matrix.hpp"
#include "esp_logger.hpp"
#include "maze.hpp"
#include "mouse.hpp"
#include "util.hpp"
#include "pid.hpp"

class Ratatoskr : public Mouse {
    private:
    GearMotor &m_motor_left;
    GearMotor &m_motor_right;
    ToF &m_tof_left;
    ToF &m_tof_front_left;
    ToF &m_tof_front_right;
    ToF &m_tof_right;
    MPU6050 &m_gyro;
    bool too_close_front(uint16_t front_left_dist, uint16_t front_right_dist);
    void moveStraightMM(float mm);
    inline void stop();
    inline void coast();
    inline void safe_stop();
    
    PID m_pid_encoders{0.75, 0.8, 0.1};
    PID m_pid_tof_sides{1.1, 0.05, 0.4};

   public:
    Ratatoskr(GearMotor &motor_left, GearMotor &motor_right, ToF &tof_left,
              ToF &tof_front_left, ToF &tof_front_right, ToF &tof_right,
              MPU6050 &gyro /*, LEDMatrix &screen*/);

    static const int MIN_TURN_PWM = 185;
    static const int MAX_TURN_PWM = 195;
    
    static const int STOP_DISTANCE = 40;
    
    static const u_int8_t FORWARD_PWM = 190;

    static const u_int8_t WIDTH_MM = 68;

    // So one revolution is 31*pi mm = 97.4 mm
    static const u_int8_t WHEEL_DIAMETER_MM = 31;

    // So encooder counts per mm = counts per rev / (WHEEL_DIAMETER_MM * pi)
    static constexpr float ENCODER_COUNTS_PER_MM =
        190.0f / (WHEEL_DIAMETER_MM * PI);

    static constexpr float MM_PER_CELL = 180.0;

    static constexpr float PWM_UPDATE_RATIO = .6f;

    // front considered blocked if any front ToF < this
    static const uint16_t FRONT_WALL_MM = 110;

    // side considered blocked if side ToF < this
    static const uint16_t SIDE_WALL_MM = 90;

    static constexpr float MAX_PWM_CORRECTION = 30.0f;
    static constexpr uint16_t TARGET_SIDE_MM = 50;     // desired side distance at sensor
    static constexpr uint16_t MAX_TOF_VALID_MM = 120;  // beyond this: treat as "no wall"
        
    static constexpr int TURN_TIME_LIMIT = 1500;
    static constexpr float TURN_TRESHOLD = 0.2f;
    
    /**
     * move @distance cells forward with PID control
     */
    virtual void moveForward(int distance = 1);

    /**
     * turn @angle degrees in counterclockwise direction
     */
    virtual void turn(int angle);

    /**
     * Front wall if either front-left or front-right ToF sees something closer
     * than FRONT_WALL_MM. Ignores zero readings (sensor not ready).
     */
    virtual bool wallFront();
    
    /**
     * Detect if a wall is towards the right.
     */
    virtual bool wallRight();
    
    /**
     * Detect if a wall is towards the left.
     */
    virtual bool wallLeft();

    virtual void update_visuals(Maze &maze);
    virtual void log(std::string msg);

    virtual bool wasReset();
    virtual void ackReset();
};
