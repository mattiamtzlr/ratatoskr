#include <Arduino.h>

#include "esp32-hal-ledc.h"
#include "oled.hpp"
#include "pins.hpp"
#include "ratatoskr.hpp"
#include "solver.hpp"
#include "util.hpp"

// I2C addresses for the ToF sensors
const uint8_t TOF_LEFT_ADDRESS = 0x30;
const uint8_t TOF_FRONT_LEFT_ADDRESS = 0x31;
const uint8_t TOF_FRONT_RIGHT_ADDRESS = 0x32;
const uint8_t TOF_RIGHT_ADDRESS = 0x33;

const MODE mode = TESTING;  // TODO: Right now you have to change this by hand.

ToF tof_left = ToF(LEFT, TOF_LEFT_ADDRESS, TOF_LEFT_XSHUT);
ToF tof_front_left =
    ToF(FRONT_LEFT, TOF_FRONT_LEFT_ADDRESS, TOF_FRONT_LEFT_XSHUT);
ToF tof_front_right =
    ToF(FRONT_RIGHT, TOF_FRONT_RIGHT_ADDRESS, TOF_FRONT_RIGHT_XSHUT);
ToF tof_right = ToF(RIGHT, TOF_RIGHT_ADDRESS, TOF_RIGHT_XSHUT);

MPU6050 gyro = MPU6050();

OLED oled = OLED();

// encoder_sign is the last argument (default = 1). Determines the direction
GearMotor motor_left(MOTOR_L_IN1, MOTOR_L_IN2, 0, 1,  // PWM channels
                     ENC_L_OUT1, ENC_L_OUT2,
                     255,  // max PWM
                     +1    // encoder_sign (I tested)
);

GearMotor motor_right(MOTOR_R_IN1, MOTOR_R_IN2, 2, 3,  // PWM channels
                      ENC_R_OUT1, ENC_R_OUT2,
                      255,  // max PWM
                      -1    // encoder_sign (I tested)
);

Maze maze;
Ratatoskr rat(motor_left, motor_right, tof_left, tof_front_left,
              tof_front_right, tof_right, gyro, oled);
Solver solver(rat, maze);

void setup() {
    // ToF XSHUT pins
    pinMode(TOF_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_RIGHT_XSHUT, OUTPUT); 

    Wire.begin();
    delay(500);
    Serial.begin(115200);
    delay(500);
    gyro.begin();
    delay(500);
    oled.begin();
    delay(500);

    tof_left.begin();
    tof_front_left.begin();
    tof_right.begin();
    tof_front_right.begin();

    // ---- ENCODER INTERRUPT SETUP (integrated with GearMotor) ----
    // Use ESP32's attachInterruptArg so each motor instance gets its own ISR
    // arg
    attachInterruptArg(ENC_L_OUT1, GearMotor::isr_trampoline, &motor_left,
                       RISING);

    attachInterruptArg(ENC_R_OUT1, GearMotor::isr_trampoline, &motor_right,
                       RISING);
    // -------------------------------------------------------------

#ifdef CALIBRATE
    tof_left.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
    tof_front_left.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    tof_front_right.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    tof_right.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
#endif

    switch (mode) {
        case DEBUG: {
            oled.mode = DEBUG;
            /* WARN: no break on purpose cause we want to fall through to RUN */
        }

        case RUN: {
            // Push target to maze
            maze.targets.push_back(Position(2, 2));
            // maze.targets.push_back(Position(7, 8));
            // maze.targets.push_back(Position(8, 7));
            // maze.targets.push_back(Position(8, 8));

            solver.solve();  // Run from start to target

            // Push start to maze
            maze.targets.clear();
            maze.targets.push_back(Position(0, 0));

            solver.solve();  // Run from target to start
            break;
        }

        case DUMP_LOG: {
            ESPLogger::export_logs();
            ESPLogger::clear_logs();
            break;
        }

        case TESTING: {
            oled.mode = DEBUG;
            while (true) {
                rat.moveForward(180);
                delay(2000);
            }
        }
    }
}

void loop() {}  // DO NOT USE THIS, WEIRD BEHAVIOR WITH LEDC AND PINS!!!!!
