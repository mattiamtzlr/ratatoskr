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
	GearMotor(int pin_1, int pin_2, int encoder_pin_1, int encoder_pin_2, int max_pwm);
	int set_speed(int speed);
	int forward(int speed);
	int backward(int speed);
	int coast();
	int brake();
	int stop();
private:
	const int PIN_1;
	const int PIN_2;
	const int ENCODER_PIN_1;
	const int ENCODER_PIN_2;
	const int MAX_PWM;
	int rpm;
	DrivingMode mode;
};
