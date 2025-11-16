#pragma once
#include "util.hpp"
#include <Arduino.h>
#include <Wire.h>

// Defalut address for MPU6050
static constexpr uint8_t MPU6050_ADDRESS = 0x68;

// MPU6050 Registers Addresses 
static constexpr uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
static constexpr uint8_t MPU6050_REG_ACCEL_XOUT_H = 0x3B;
static constexpr uint8_t MPU6050_REG_GYRO_XOUT_H = 0x43;
static constexpr uint8_t MPU6050_REG_WHO_AM_I = 0x68;
static constexpr uint8_t MPU6050_REG_ACCEL_CONFIG = 0x1C;
static constexpr uint8_t MPU6050_REG_GYRO_CONFIG = 0x1B;


// Sensitivity for gyroscope
enum GyroSensitivity{
    GYRO_RANGE_250_DEG = 0b00,
    GYRO_RANGE_500_DEG = 0b01,
    GYRO_RANGE_1000_DEG = 0b10,
    GYRO_RANGE_2000_DEG = 0b11
};

// Sensitivity for accelerometer
enum AccelSensitivity {
    ACCEL_RANGE_2G = 0b00,
    ACCEL_RANGE_4G = 0b01,
    ACCEL_RANGE_8G = 0b10,
    ACCEL_RANGE_16G = 0b11
};

// Holds 3-axis reading 
struct Vector3D {
    float x;
    float y;
    float z;
};

class MPU6050{
public:
    MPU6050(uint8_t addr = MPU6050_ADDRESS);
    
    // Setup gyroscope and accelerometer
    bool begin();

    void setGyroSensitivity(GyroSensitivity range);
    void setAccelSensitivity(AccelSensitivity range);

    Vector3D readRawGyro();
    Vector3D readRawAccel();
    
    Vector3D readScaledGyro(); // Rerturns deg/s
    Vector3D readScaledAccel(); // Returns G's

    // Get current angle in deg (around z axis)
    float getAngle(unsigned long t_now, unsigned long t_last);

    // Calc offsets
    void calibrateGyro();

private:
    uint8_t m_addr;
    float m_angle;
    float m_accelScale; // Sensitivity scale factor
    float m_gyroScale;  // Sensitivity scale factor
    
    // Low-level helper functions for I2C communication
    void writeByte(uint8_t reg, uint8_t data);
    uint8_t readByte(uint8_t reg);
    void readBytes(uint8_t reg, uint8_t count, uint8_t* dest);

    // Helper to convert raw reading to float with scale
    Vector3D rawToScaled(int16_t x, int16_t y, int16_t z, float scale);
};
