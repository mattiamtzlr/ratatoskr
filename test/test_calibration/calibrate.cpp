#include <Arduino.h>

#include "pins.hpp"
#include "ratatoskr.hpp"
#include "solver.hpp"

// I2C addresses for the ToF sensors
const uint8_t TOF_LEFT_ADDRESS = 0x30;
const uint8_t TOF_FRONT_LEFT_ADDRESS = 0x31;
const uint8_t TOF_FRONT_RIGHT_ADDRESS = 0x32;
const uint8_t TOF_RIGHT_ADDRESS = 0x33;

ToF tof_left = ToF(LEFT, TOF_LEFT_ADDRESS, TOF_LEFT_XSHUT);
ToF tof_left_front =
    ToF(FRONT_LEFT, TOF_FRONT_LEFT_ADDRESS, TOF_FRONT_LEFT_XSHUT);
ToF tof_right_front =
    ToF(FRONT_RIGHT, TOF_FRONT_RIGHT_ADDRESS, TOF_FRONT_RIGHT_XSHUT);
ToF tof_right = ToF(RIGHT, TOF_RIGHT_ADDRESS, TOF_RIGHT_XSHUT);

MPU6050 gyro = MPU6050();

Maze maze;
GearMotor motor_left(MOTOR_L_IN1, MOTOR_L_IN2, ENC_L_OUT1, ENC_L_OUT2, 50);
GearMotor motor_right(MOTOR_R_IN1, MOTOR_R_IN2, ENC_R_OUT1, ENC_R_OUT2, 50);

Ratatoskr rat(motor_left, motor_right, tof_left, tof_left_front,
              tof_right_front, tof_right, gyro);
// Solver solver(rat, maze);

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
    tof_left_front.begin();
    tof_right.begin();
    tof_right_front.begin();

    int count = 0;
    Serial.println("ToF Readings (mm):");
    while (count++ < 500) {
        Serial.print("Left: ");
        Serial.print(tof_left.read());
        Serial.print("  Front Left: ");
        Serial.print(tof_left_front.read());
        Serial.print("  Front Right: ");
        Serial.print(tof_right_front.read());
        Serial.print("  Right: ");
        Serial.println(tof_right.read());
        delay(50);
    }

    Serial.println("ToF now calibrated.");
    delay(1000);
    tof_left.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
    tof_left_front.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    tof_right.calibrate_sensor(TOF_SIDE_EXPECTED_MM);
    tof_right_front.calibrate_sensor(TOF_FRONT_EXPECTED_MM);
    count = 0;
    while (count++ < 500) {
        Serial.print("Left: ");
        Serial.print(tof_left.read());
        Serial.print("  Front Left: ");
        Serial.print(tof_left_front.read());
        Serial.print("  Front Right: ");
        Serial.print(tof_right_front.read());
        Serial.print("  Right: ");
        Serial.println(tof_right.read());
        delay(50);
    }

    Serial.println("Gyro without calibration:");
    count = 0;
    while (count++ < 500) {
        Vector3D g = gyro.readScaledGyro();
        Serial.print("Gx: ");
        Serial.print(g.x);
        Serial.print("  Gy: ");
        Serial.print(g.y);
        Serial.print("  Gz: ");
        Serial.println(g.z);
        delay(50);
    }
    Serial.println("Calibrating gyro...");
    gyro.calibrateGyro();
    count = 0;
    while (count++ < 500) {
        Vector3D g = gyro.readScaledGyro();
        Serial.print("Gx: ");
        Serial.print(g.x);
        Serial.print("  Gy: ");
        Serial.print(g.y);
        Serial.print("  Gz: ");
        Serial.println(g.z);
        delay(50);
    }
}

void loop() {}
