#include <WiFi.h>

#include "esp_bt.h"
#include "ratatoskr.hpp"

ToF tof_left = ToF(LEFT, 0x29);
ToF tof_left_front = ToF(FRONT_LEFT, 0x28);
ToF tof_right_front = ToF(FRONT_RIGHT, 0x27);
ToF tof_right = ToF(RIGHT, 0x26);

/* left motor, pins in order of ESP => motor A resp. 1 */
#define MOTOR_L_IN1 12
#define MOTOR_L_IN2 14
#define ENC_L_OUT1 33
#define ENC_L_OUT2 32

/* right motor, pins in order of ESP => motor B resp. 2 */
#define MOTOR_R_IN1 27
#define MOTOR_R_IN2 26
#define ENC_R_OUT1 35
#define ENC_R_OUT2 34

Maze maze();
GearMotor motor_left(MOTOR_L_IN1, MOTOR_L_IN2, ENC_L_OUT1, ENC_L_OUT2, 50);
GearMotor motor_right(MOTOR_R_IN1, MOTOR_R_IN2, ENC_R_OUT1, ENC_R_OUT2, 50);

Ratatoskr rat(motor_left, motor_right, tof_left, tof_left_front,
              tof_right_front, tof_right);

void disableWireless() {
    WiFi.mode(WIFI_OFF);
    WiFi.setSleep(true);
    if (btStarted()) btStop();
    esp_bt_controller_disable();
}

void setup() {
    Serial.begin(115200);

    Wire.begin();
    disableWireless();

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
