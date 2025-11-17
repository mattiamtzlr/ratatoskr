#include "VL53L1X.h"
#include "ToF.hpp"
#include "pins.hpp"
#include "gear_motor.hpp"


void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Starting ToF sensors...");
    Wire.begin();
    delay(100);

    // Setup XSHUT pins
    pinMode(TOF_RIGHT_XSHUT,        OUTPUT);
    pinMode(TOF_FRONT_RIGHT_XSHUT,  OUTPUT);
    pinMode(TOF_FRONT_LEFT_XSHUT,   OUTPUT);
    pinMode(TOF_LEFT_XSHUT,         OUTPUT);   


    delay(100);

    ToF tof_left        = ToF(LEFT,         ADDR_LEFT,          TOF_LEFT_XSHUT); 
    ToF tof_front_left  = ToF(FRONT_LEFT,   ADDR_FRONT_LEFT,    TOF_FRONT_LEFT_XSHUT);
    ToF tof_front_right = ToF(FRONT_RIGHT,  ADDR_FRONT_RIGHT,   TOF_FRONT_RIGHT_XSHUT);
    ToF tof_right       = ToF(RIGHT,        ADDR_RIGHT,         TOF_RIGHT_XSHUT);


    Serial.println("Starting ToF continuous readings...");
    tof_left.start();
    delay(100);
    while (!tof_left.dataReady()) {
        delay(10);
        Serial.println("Waiting for ToF left...");
    }
    Serial.println(tof_left.dataReady() ? "ToF left ready" : "ToF Left not ready");
    
    tof_front_left.start();
    while (!tof_front_left.dataReady()) {
        delay(10);
    }
    Serial.println(tof_front_left.dataReady() ? "ToF fl ready" : "ToF fl not ready");

    tof_front_right.start();
    while(!tof_front_right.dataReady()) {
        delay(10);
    }
    Serial.println(tof_front_right.dataReady() ? "ToF fr ready" : "ToF fr not ready");

    tof_right.start();
    while (!tof_right.dataReady()) {
        delay(10);
    }
    Serial.println(tof_right.dataReady() ? "ToF right ready" : "ToF right not ready");


    while (true){
        Serial.print("Left: "); Serial.println(tof_left.read());
        Serial.print("Front Left: "); Serial.println(tof_front_left.read());
        Serial.print("Front Right: "); Serial.println(tof_front_right.read());
        Serial.print("Right: "); Serial.println(tof_right.read());
        Serial.println("-----");
        delay(100);
    }

}

void loop() {
}
