#include <Arduino.h>

#include "gear_motor.hpp"
// Basic sketch for trying out the Adafruit DRV8871 Breakout
#define MOTOR_IN1 18
#define MOTOR_IN2 19
#define ENC_OUT1 26
#define ENC_OUT2 25

GearMotor motor(MOTOR_IN1, MOTOR_IN2, ENC_OUT1, ENC_OUT2, 50);

void setup() {
    Serial.begin(115200);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
}
void loop() {
    motor.stop();
    Serial.println("counterclockwise");
    motor.spin_ccw(60);
    delay(1000);
    Serial.println(motor.get_rpm());
    delay(1000);
    motor.stop();
    Serial.println("stop");
    Serial.println("clockwise");
    motor.spin_cw(60);
    delay(1000);
    Serial.println(motor.get_rpm());
    delay(1000);
    /*
        Serial.println("counterclockwise");
    delay(1000);
    motor.spin_cw(100);
    delay(1000);
    motor.spin_cw(15);
    delay(1000);
    motor.stop();
        */
}
