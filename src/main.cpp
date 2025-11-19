#include <Arduino.h>

#include "pins.hpp"
#include "ratatoskr.hpp"
#include "solver.hpp"

// I2C addresses for the ToF sensors
const uint8_t TOF_LEFT_ADDRESS = 0x30;
const uint8_t TOF_FRONT_LEFT_ADDRESS = 0x31;
const uint8_t TOF_FRONT_RIGHT_ADDRESS = 0x32;
const uint8_t TOF_RIGHT_ADDRESS = 0x33;

const MODE mode = RUN;  // TODO: Right now you have to change this by hand.

ToF tof_left = ToF(LEFT, TOF_LEFT_ADDRESS, TOF_LEFT_XSHUT);
ToF tof_front_left =
    ToF(FRONT_LEFT, TOF_FRONT_LEFT_ADDRESS, TOF_FRONT_LEFT_XSHUT);
ToF tof_front_right =
    ToF(FRONT_RIGHT, TOF_FRONT_RIGHT_ADDRESS, TOF_FRONT_RIGHT_XSHUT);
ToF tof_right = ToF(RIGHT, TOF_RIGHT_ADDRESS, TOF_RIGHT_XSHUT);

MPU6050 gyro = MPU6050();

Maze maze;
GearMotor motor_left(MOTOR_L_IN1, MOTOR_L_IN2, ENC_L_OUT1, ENC_L_OUT2, 50);
GearMotor motor_right(MOTOR_R_IN1, MOTOR_R_IN2, ENC_R_OUT1, ENC_R_OUT2, 50);


Ratatoskr rat(motor_left, motor_right, tof_left, tof_front_left,
              tof_front_right, tof_right, gyro);
Solver solver(rat, maze);

void setup() {
    // Motor pins for left
    pinMode(MOTOR_L_IN1, OUTPUT);
    pinMode(MOTOR_L_IN2, OUTPUT);
    pinMode(ENC_L_OUT1, INPUT);
    pinMode(ENC_L_OUT2, INPUT);
    attachInterruptArg(digitalPinToInterrupt(ENC_L_OUT2),
                       motor_left.isr_trampoline, &motor_left, RISING);

    // Motor pins for right
    pinMode(MOTOR_R_IN1, OUTPUT);
    pinMode(MOTOR_R_IN2, OUTPUT);
    pinMode(ENC_R_OUT1, INPUT);
    pinMode(ENC_R_OUT2, INPUT);
    attachInterruptArg(digitalPinToInterrupt(ENC_R_OUT2),
                       motor_right.isr_trampoline, &motor_right, RISING);

    // ToF XSHUT pins
    pinMode(TOF_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_RIGHT_XSHUT, OUTPUT);

    Wire.begin();
    Serial.begin(115200);
    gyro.begin();
    delay(1000);

    tof_left.begin();
    tof_front_left.begin();
    tof_right.begin();
    tof_front_right.begin();

    #ifdef CALIBRATE
    tof_left.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
    tof_front_left.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    tof_front_right.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    tof_right.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
    #endif

    switch (mode) {
        case RUN: {
            // Push target to maze
            maze.targets.push_back(Position(7, 7));
            maze.targets.push_back(Position(7, 8));
            maze.targets.push_back(Position(8, 7));
            maze.targets.push_back(Position(8, 8));

            solver.solve();  // Run from start to target

            // Push start to maze
            maze.targets.clear();
            maze.targets.push_back(Position(0, 0));

            solver.solve();  // Run from target to start
            break;
        }

        case DUMP_LOG: {
            while (!Serial.available());
            rat.export_logs();
            break;
        }

        case TESTING: break;
    }
}

void loop() {}
