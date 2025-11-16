#include "esp_bt.h"
#include "ratatoskr.hpp"
#include "pins.hpp"

#define TOF_LEFT_XSHUT          23
#define TOF_FRONT_LEFT_XSHUT    19
#define TOF_FRONT_RIGHT_XSHUT   18
#define TOF_RIGHT_XSHUT         15

// I2C addresses for the ToF sensors
const uint8_t TOF_LEFT        = 0x30;
const uint8_t TOF_FRONT_LEFT  = 0x31;
const uint8_t TOF_FRONT_RIGHT = 0x32;
const uint8_t TOF_RIGHT       = 0x33;

ToF tof_left = ToF(LEFT, TOF_LEFT, TOF_LEFT_XSHUT);
ToF tof_left_front = ToF(FRONT_LEFT, TOF_FRONT_LEFT,TOF_FRONT_LEFT_XSHUT);
ToF tof_right_front = ToF(FRONT_RIGHT, TOF_FRONT_RIGHT, TOF_FRONT_RIGHT_XSHUT);
ToF tof_right = ToF(RIGHT, TOF_RIGHT, TOF_RIGHT_XSHUT);

MPU6050 gyro = MPU6050();

Maze maze();
GearMotor motor_left(MOTOR_L_IN1, MOTOR_L_IN2, ENC_L_OUT1, ENC_L_OUT2, 50);
GearMotor motor_right(MOTOR_R_IN1, MOTOR_R_IN2, ENC_R_OUT1, ENC_R_OUT2, 50);

Ratatoskr rat(motor_left, motor_right, tof_left, tof_left_front,
              tof_right_front, tof_right, gyro);

void disableWireless() {
    if (btStarted()) btStop();
    esp_bt_controller_disable();
}

void setup() {
    Serial.begin(115200);

    Wire.begin();
    disableWireless();
    gyro.begin();

    pinMode(MOTOR_L_IN1, OUTPUT);
    pinMode(MOTOR_L_IN2, OUTPUT);
    pinMode(ENC_L_OUT1, INPUT);
    pinMode(ENC_L_OUT2, INPUT);
    pinMode(MOTOR_R_IN1, OUTPUT);
    pinMode(MOTOR_R_IN2, OUTPUT);
    pinMode(ENC_R_OUT1, INPUT);
    pinMode(ENC_R_OUT2, INPUT);

    attachInterruptArg(digitalPinToInterrupt(ENC_L_OUT2),
                       motor_left.isr_trampoline, &motor_left, RISING);
    attachInterruptArg(digitalPinToInterrupt(ENC_R_OUT2),
                       motor_right.isr_trampoline, &motor_right, RISING);
}

void loop() {
    rat.moveForward(2);
    delay(2000);
    rat.turn(90);
    delay(2000);
}
