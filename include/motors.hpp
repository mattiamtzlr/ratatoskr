#pragma once
// Adafruit DRV8871 DC Motor Driver Breakout Board
enum DrivingMode {
	FORWARD,
	BACKWARD,
	BRAKE,
	COAST,
	STOP
};

class GearMotor {
public:
	GearMotor(int pin_1, int pin_2);
	int set_speed(int speed);
	int forward(int speed);
	int backward(int speed);
	int coast();
	int brake();
	int stop();
private:
	int pin_1;
	int pin_2;
	int max_pwm;
	DrivingMode mode;
};
