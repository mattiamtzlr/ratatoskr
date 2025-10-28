#include "../include/motors.hpp"
#include <Arduino.h>
#include <cstdlib>

/**
* TODO: Method description
*/
GearMotor::GearMotor(int in1, int in2, int encoder_pin_1, int encoder_pin_2, int max_pwm) : IN1(in1), IN2(in2), ENCODER_PIN_1(encoder_pin_1), ENCODER_PIN_2(encoder_pin_2) {
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);

	pinMode(ENCODER_PIN_1, INPUT_PULLUP);
	pinMode(ENCODER_PIN_2, INPUT_PULLUP);
	// TODO: finish this? i.e., handle interrupts, etc
}

int GearMotor::get_rpm() {
	return m_rpm;
}

/**
* TODO: Method description
*/
int GearMotor::set_speed(int rpm) {
	float rpm_perc = (float)(rpm - MIN_RPM) / MAX_RPM;
	int pwm = (int)(MIN_PWM + (MAX_PWM - MIN_PWM) * rpm_perc);
	switch(m_mode) {
		case FORWARD:
			analogWrite(IN2, pwm);
			break;
		case BACKWARD:
			analogWrite(IN1, pwm);
			break;
		default:
			break;
	}
	// TODO: Fix this because this is probably not right...
}

/**
* TODO: Method description
*/
int GearMotor::forward(int speed) {
	digitalWrite(IN1, LOW);
	m_mode = FORWARD;
	set_speed(speed);
}

/**
* TODO: Method description
*/
int GearMotor::backward(int speed) {
	digitalWrite(IN2, LOW);
	m_mode = BACKWARD;
	set_speed(speed);
}

/**
* TODO: Method description
*/
int GearMotor::coast() {
	// TODO: Method implementation
}

/**
* TODO: Method description
*/
int GearMotor::brake() {
	// TODO: Method implementation
}

/**
* TODO: Method description
*/
int GearMotor::stop() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
}

