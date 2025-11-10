#include "tof.hpp"

ToF tof = ToF(LEFT, 0x29);  // rip xshut and irq

#include "gear_motor.hpp"

#define MOTOR_IN1 18
#define MOTOR_IN2 19
#define ENC_OUT1 5
#define ENC_OUT2 21

GearMotor motor(MOTOR_IN1, MOTOR_IN2, ENC_OUT1, ENC_OUT2, 50);

void setup() {
    Serial.begin(115200);

    Wire.begin();

    tof.start(&Wire);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(ENC_OUT1, INPUT);
    pinMode(ENC_OUT2, INPUT);

    attachInterruptArg(digitalPinToInterrupt(ENC_OUT2), motor.isr_trampoline,
                       &motor, RISING);
}

void loop() {
    Serial.println("starting...");
    motor.stop();
    motor.spin_ccw(60);
    delay(2000);
    while (1) {
        delay(100);
        if (tof.dataReady()) Serial.println(tof.read());
        Serial.println(motor.get_rpm());
    }
    motor.stop();
}
