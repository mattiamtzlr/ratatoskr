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
	int get_rpm();
private:
	static constexpr int HIGH = 1;
	static constexpr int LOW = 0;
	static constexpr int MAX_PWM = 255; // TODO: Change this before running on real hardware!
	static constexpr int MIN_PWM = 40; // TODO: Change this before running on real hardware!
	static constexpr int MAX_RPM = 500; // TODO: Change this before running on real hardware!
	static constexpr int MIN_RPM = 0; // TODO: Change this before running on real hardware!

	const int IN1;
	const int IN2;
	const int ENCODER_PIN_1;
	const int ENCODER_PIN_2;
	int m_rpm;
	DrivingMode m_mode;
};
