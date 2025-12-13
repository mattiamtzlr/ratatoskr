#include "gyro.hpp"

#include <math.h>

#include "esp32-hal.h"
#include "esp_logger.hpp"

using namespace ESPLogger;

// Scale convertions for Accel
static constexpr float ACCEL_SCALE_2G = 16384.0f;
static constexpr float ACCEL_SCALE_4G = 8192.0f;
static constexpr float ACCEL_SCALE_8G = 4096.0f;
static constexpr float ACCEL_SCALE_16G = 2048.0f;

// Scale convertions for Gyro
static constexpr float GYRO_SCALE_250_DEG = 131.0f;
static constexpr float GYRO_SCALE_500_DEG = 65.5f;
static constexpr float GYRO_SCALE_1000_DEG = 32.8f;
static constexpr float GYRO_SCALE_2000_DEG = 16.4f;

float X_OFFSET = 0;
float Y_OFFSET = 0;
float Z_OFFSET = 0;

std::map<AccelSensitivity, float> rangeToScaleAccel = {
    {ACCEL_RANGE_2G, ACCEL_SCALE_2G},
    {ACCEL_RANGE_4G, ACCEL_SCALE_4G},
    {ACCEL_RANGE_8G, ACCEL_SCALE_8G},
    {ACCEL_RANGE_16G, ACCEL_SCALE_16G},
};

std::map<GyroSensitivity, float> rangeToScaleGyro = {
    {GYRO_RANGE_250_DEG, GYRO_SCALE_250_DEG},
    {GYRO_RANGE_500_DEG, GYRO_SCALE_500_DEG},
    {GYRO_RANGE_1000_DEG, GYRO_SCALE_1000_DEG},
    {GYRO_RANGE_2000_DEG, GYRO_SCALE_2000_DEG},
};

MPU6050::MPU6050(uint8_t addr)
    : m_addr(addr),
      m_accelScale(ACCEL_SCALE_16G),
      m_gyroScale(GYRO_SCALE_250_DEG),
      m_angle(0),
      m_t_now(micros()),
      m_t_last(micros()) {}

bool MPU6050::begin() {
    // Wake up the MPU-6050
    writeByte(MPU6050_REG_PWR_MGMT_1, 0x00);
    delay(10);
    // Who Am I check
    uint8_t device_id = readByte(MPU6050_REG_WHO_AM_I);
    delay(10);
    if (device_id != 0x68) {
        return false;
    }
    delay(1000);
    calibrateGyro();
    // delay(1000);
    return true;
}

void MPU6050::setAccelSensitivity(AccelSensitivity range) {
    m_accelScale = rangeToScaleAccel[range];
    // AFS_SEL are bits 3-4 in ACCEL_CONFIG
    uint8_t cfg = readByte(MPU6050_REG_ACCEL_CONFIG);
    cfg &= ~0x18;  // clear bits 3 and 4
    cfg |= (uint8_t(range) & 0x03) << 3;
    writeByte(MPU6050_REG_ACCEL_CONFIG, cfg);
}

void MPU6050::setGyroSensitivity(GyroSensitivity range) {
    m_gyroScale = rangeToScaleGyro[range];

    writeByte(MPU6050_REG_GYRO_CONFIG, range);
}

uint8_t MPU6050::readByte(uint8_t reg) {
    uint8_t data = 0;

    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(true);

    Wire.requestFrom(m_addr, 1);

    if (Wire.available()) {
        data = Wire.read();
    }

    return data;
}

void MPU6050::readBytes(uint8_t reg, uint8_t count, uint8_t* dest) {
    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(false);

    uint8_t received = Wire.requestFrom(m_addr, count);

    if (received == count) {
        for (int i = 0; i < count; i++) {
            if (Wire.available()) {
                dest[i] = Wire.read();
            }
        }
    }
}

void MPU6050::writeByte(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
}

Vector3D MPU6050::rawToScaled(int16_t x, int16_t y, int16_t z, float scale) {
    Vector3D vec = {(float)x / scale, (float)y / scale, (float)z / scale};
    return vec;
}

Vector3D MPU6050::readRawAccel() {
    // 3 Mesurements of 2 bytes each
    const int BYTES_TO_READ = 6;
    uint8_t raw_data[BYTES_TO_READ];

    readBytes(MPU6050_REG_ACCEL_XOUT_H, BYTES_TO_READ, raw_data);
    int16_t x = int16_t((raw_data[0] << 8) | raw_data[1]);
    int16_t y = int16_t((raw_data[2] << 8) | raw_data[3]);
    int16_t z = int16_t((raw_data[4] << 8) | raw_data[5]);

    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readRawGyro() {
    // 3 Mesurements of 2 bytes each
    const int BYTES_TO_READ = 6;
    uint8_t raw_data[BYTES_TO_READ];

    readBytes(MPU6050_REG_GYRO_XOUT_H, BYTES_TO_READ, raw_data);
    int16_t x = int16_t((raw_data[0] << 8) | raw_data[1]);
    int16_t y = int16_t((raw_data[2] << 8) | raw_data[3]);
    int16_t z = int16_t((raw_data[4] << 8) | raw_data[5]);

    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readScaledAccel() {
    Vector3D raw = readRawAccel();
    return rawToScaled(raw.x, raw.y, raw.z, m_accelScale);
}

Vector3D MPU6050::readScaledGyro() {
    Vector3D raw = readRawGyro();
    Vector3D scaled = rawToScaled(raw.x, raw.y, raw.z, m_gyroScale);
    Vector3D scaled_and_unbiased = {scaled.x - X_OFFSET, scaled.y - Y_OFFSET,
                                    scaled.z - Z_OFFSET};
    return scaled_and_unbiased;
}

void MPU6050::calibrateGyro() {
    float x_values = 0;
    float y_values = 0;
    float z_values = 0;
    unsigned long count = 0;
    delay(10);
    while (count < 2500) {
        Serial.println("Collecting sample " + String(count + 1) + "/2500");
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                break;
            }
        }

        Vector3D vec = readScaledGyro();
        x_values += vec.x;
        y_values += vec.y;
        z_values += vec.z;
        count++;

        delay(1);
    }

    if (count == 0) {
        X_OFFSET = 0.0f;
        Y_OFFSET = 0.0f;
        Z_OFFSET = 0.0f;
        return;
    }

    X_OFFSET = (float)x_values / (float)count;
    Y_OFFSET = (float)y_values / (float)count;
    Z_OFFSET = (float)z_values / (float)count;

}

void MPU6050::update() {
    m_t_last = m_t_now;
    m_t_now = micros();
}

float MPU6050::get_current_angle() { return m_angle; }

float MPU6050::get_next_angle() {
    Vector3D velocities = readScaledGyro();
    float dt =
        (float)(m_t_now - m_t_last) * 1e-6f;  // t_now/t_last must be micros()

    if (dt <= 0.0f) {
        return m_angle;
    }

    float change = velocities.z * dt;  // degrees
    m_angle += change;
    return m_angle;
}
