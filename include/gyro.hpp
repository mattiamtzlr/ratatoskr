#pragma once

#include <Wire.h>

// Defalut address for MPU6050
static const uint8_t MPU6050_ADDRESS = 0x68;

// MPU6050 Registers Addresses 
static const uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
static const uint8_t MPU6050_REG_ACCEL_XOUT_H = 0x3B;
static const uint8_t MPU6050_REG_GYRO_XOUT_H = 0x43;
static const uint8_t MPU6050_REG_WHO_AM_I = 0x75;

// Sensitivity for gyroscope
enum GyroSensitivity{
    GYRO_RANGE_250_DEG,
    GYRO_RANGE_500_DEG,
    GYRO_RANGE_1000_DEG,
    GYRO_RANGE_2000_DEG
};

// Sensitivity for accelerometer
enum AccelSensitivity {
    ACCEL_RANGE_2G,
    ACCEL_RANGE_4G,
    ACCEL_RANGE_8G,
    ACCEL_RANGE_16G
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
    bool init();

    void setGyroSensitivity(GyroSensitivity range);
    void setAccelSensitivity(AccelSensitivity range);

    Vector3D readRawGyro();
    Vector3D readRawAccel();

    
    Vector3D readRawGyro(); // Rerturns deg/s
    Vector3D readScaledAccel(); // Returns G's

private:
    uint8_t m_addr;
    float m_accelScale; // Sensitivity scale factor
    float m_gyroScale;  // Sensitivity scale factor
    
    // Low-level helper functions for I2C communication
    void writeByte(uint8_t reg, uint8_t data);
    uint8_t readByte(uint8_t reg);
    void readBytes(uint8_t reg, uint8_t count, uint8_t* dest);

    // Helper to convert raw reading to float with scale
    Vector3D rawToScaled(int16_t x, int16_t y, int16_t z, float scale);
};