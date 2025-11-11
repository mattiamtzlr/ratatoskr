#include "gyro.hpp"

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


MPU6050::MPU6050(uint8_t addr) : m_addr(addr), m_accelScale(ACCEL_RANGE_16G), m_gyroScale(GYRO_SCALE_250_DEG){}

bool MPU6050::begin(){
    // Wake up the MPU-6050
    writeByte(MPU6050_REG_PWR_MGMT_1, 0x00);
    
    // Who Am I check
    uint8_t device_id = readByte(MPU6050_REG_WHO_AM_I);
    
    if (device_id != 0x68) {
        return false;
    }
    return true;
}

void MPU6050::setAccelSensitivity(AccelSensitivity range){
    m_accelScale = rangeToScaleAccel[range];

    writeByte(MPU6050_REG_ACCEL_CONFIG, range);
}

void MPU6050::setGyroSensitivity(GyroSensitivity range){
    m_gyroScale = rangeToScaleGyro[range];

    writeByte(MPU6050_REG_GYRO_CONFIG, range);
}

uint8_t MPU6050::readByte(uint8_t reg){
    uint8_t data;

    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(true);

    Wire.requestFrom(m_addr, 1);

    if(Wire.available()){
        data = Wire.read();
    }

    return data;
}


void MPU6050::readBytes(uint8_t reg, uint8_t count, uint8_t* dest){
    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(false);

    uint8_t received = Wire.requestFrom(m_addr, count);

    if (received == count) {
        for (int i = 0; i < count; i++) {
            dest[i] = Wire.read();
        }
    }
}

void MPU6050::writeByte(uint8_t reg, uint8_t data){
    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission(true);
}

Vector3D MPU6050::rawToScaled(int16_t x, int16_t y, int16_t z, float scale){
    Vector3D vec = {(float)x/scale, (float)y/scale, (float)z/scale};
    return vec;
}

Vector3D MPU6050::readRawAccel(){
    // 3 Mesurements of 2 bytes each
    const int BYTES_TO_READ = 6;
    uint8_t raw_data[BYTES_TO_READ];

    readBytes(MPU6050_REG_ACCEL_XOUT_H, BYTES_TO_READ, raw_data);
    uint16_t x = uint16_t(raw_data[0] << 8 | raw_data[1]);
    uint16_t y = uint16_t(raw_data[2] << 8 | raw_data[3]);
    uint16_t z = uint16_t(raw_data[4] << 8 | raw_data[5]);
    
    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readRawGyro(){
    // 3 Mesurements of 2 bytes each
    const int BYTES_TO_READ = 6;
    uint8_t raw_data[BYTES_TO_READ];

    readBytes(MPU6050_REG_GYRO_XOUT_H, BYTES_TO_READ, raw_data);
    uint16_t x = uint16_t(raw_data[0] << 8 | raw_data[1]);
    uint16_t y = uint16_t(raw_data[2] << 8 | raw_data[3]);
    uint16_t z = uint16_t(raw_data[4] << 8 | raw_data[5]);
    
    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readScaledAccel(){
    Vector3D raw = readRawAccel();
    return rawToScaled(raw.x, raw.y, raw.z, m_accelScale);
}

Vector3D MPU6050::readScaledGyro(){
    Vector3D raw = readRawGyro();
    return rawToScaled(raw.x, raw.y, raw.z, m_gyroScale);
}
