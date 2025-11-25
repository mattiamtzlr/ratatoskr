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

   public:
    Ratatoskr(GearMotor &motor_left, GearMotor &motor_right, ToF &tof_left,
              ToF &tof_front_left, ToF &tof_front_right, ToF &tof_right,
              MPU6050 &gyro /*, LEDMatrix &screen*/);

    // static const float PERIOD;  // TODO: we compute this by recording 360/T
    static const u_int8_t TURN_PWM = 150;  // TODO: we set this arbitrarily
    static const int STOP_DISTANCE = 90;
    // static const float TIME_PER_CELL;  // TODO: how long rat goes 16 cm

    static const u_int8_t FORWARD_PWM = 200;  // TODO: we set this arbiturarily

    static const u_int8_t WIDTH_MM = 68;

    // So one revolution is 27*pi mm = 84.82 mm
    static const u_int8_t WHEEL_DIAMETER_MM = 27;

    // So encooder counts per mm = counts per rev / (WHEEL_DIAMETER_MM * pi)
    static constexpr float ENCODER_COUNTS_PER_MM =
        200.0f / (WHEEL_DIAMETER_MM * PI);

    static constexpr float MM_PER_CELL = 160.0;

    inline void stop();

    // front considered blocked if any front ToF < this
    static const uint16_t FRONT_WALL_MM = 110;

    // side considered blocked if side ToF < this
    static const uint16_t SIDE_WALL_MM = 70;

    virtual void moveForward(int distance = 1);
    virtual void turn(int angle);

    void calibrateEncoders();

    virtual bool wallFront();
    virtual bool wallRight();
    virtual bool wallLeft();

    virtual void update_visuals(Maze &maze);
    virtual void log(std::string msg);

    virtual bool wasReset();
    virtual void ackReset();
};
