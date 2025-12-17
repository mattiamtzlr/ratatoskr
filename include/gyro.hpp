#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "util.hpp"

// Default address for MPU6050
static constexpr uint8_t MPU6050_ADDRESS = 0x68;

// MPU6050 Registers Addresses
static constexpr uint8_t MPU6050_REG_PWR_MGMT_1 = 0x6B;
static constexpr uint8_t MPU6050_REG_ACCEL_XOUT_H = 0x3B;
static constexpr uint8_t MPU6050_REG_GYRO_XOUT_H = 0x43;
static constexpr uint8_t MPU6050_REG_WHO_AM_I = 0x68;
static constexpr uint8_t MPU6050_REG_ACCEL_CONFIG = 0x1C;
static constexpr uint8_t MPU6050_REG_GYRO_CONFIG = 0x1B;

// Sensitivity for gyroscope
enum GyroSensitivity {
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

class MPU6050 {
   public:
    MPU6050(uint8_t addr = MPU6050_ADDRESS);
    /**
     * Setup gyroscope and accelerometer.
     * Also starts calibration.
     */
    bool begin();

    /**
     * Set the sensitivity on the sensor.
     */
    void setGyroSensitivity(GyroSensitivity range);

    /**
     * Set the sensitivity of the acceleration on the sensor.
     */
    void setAccelSensitivity(AccelSensitivity range);

    /**
     * Read raw gyro measurement.
     * Requires Wire to be set up.
     */
    Vector3D readRawGyro();

    /**
     * Read raw acceleration measurement.
     * Requires Wire to be set up.
     */
    Vector3D readRawAccel();

    /**
     * Read gyro measurement.
     * Requires Wire to be set up.
     * @returns deg/s
     */
    Vector3D readScaledGyro();

    /**
     * Read raw acceleration measurement.
     * Requires Wire to be set up.
     * @returns G's
     */
    Vector3D readScaledAccel();

    /**
     * Update the angle reading.
     * - Call this after update
     */
    float get_next_angle();

    /**
     * Getter for the current angle.
     * Does not read the sensor.
     */
    float get_current_angle();

    /**
     * Update the class internal timer for gyro readings.
     */
    void update();

    /**
     * Calibrate the gyro.
     * WARNING: Do not touch the sensor while it is calibrating
     */
    void calibrateGyro();

   private:
    uint8_t m_addr;
    float m_angle;
    float m_accelScale;  // Sensitivity scale factor
    float m_gyroScale;   // Sensitivity scale factor
    unsigned long m_t_now;
    unsigned long m_t_last;

    // Low-level helper functions for I2C communication
    void writeByte(uint8_t reg, uint8_t data);
    uint8_t readByte(uint8_t reg);
    void readBytes(uint8_t reg, uint8_t count, uint8_t* dest);

    // Helper to convert raw reading to float with scale
    Vector3D rawToScaled(int16_t x, int16_t y, int16_t z, float scale);
};
