#include "../include/gyro.hpp"
#include <Arduino.h>

MPU6050::MPU6050(uint8_t addr) : m_addr(addr){
    bool status = begin();
    if(status == 0){
        Serial.println("MPU6050 setup correctly");
    } else {
        Serial.println("Error during setup of MPU6050");
        return;
    }

}

bool MPU6050::begin(){
    Wire.beginTransmission(m_addr);
    Wire.write(MPU6050_REG_PWR_MGMT_1);
    Wire.write(0x00);
    uint8_t status = Wire.endTransmission(true);

    if(status == 0){
        return true;
    } else {
        return false;
    }
}

void MPU6050::setAccelSensitivity(AccelSensitivity range){
    switch(range){
        case ACCEL_RANGE_2G:
            m_accelScale = 16384;
            break;
        case ACCEL_RANGE_4G:
            m_accelScale = 8192;
            break;
        case ACCEL_RANGE_8G:
            m_accelScale = 4096;
            break;
        case ACCEL_RANGE_16G:
            m_accelScale = 2048;
            break;
        default:
    }

    Wire.beginTransmission(m_addr);
    Wire.write(MPU6050_REG_ACCEL_CONFIG);   
    Wire.write(range);       
    Wire.endTransmission(true);
}

void MPU6050::setGyroSensitivity(GyroSensitivity range){
    switch(range){
        case GYRO_RANGE_250_DEG:
            m_gyroScale = 131;
            break;
        case GYRO_RANGE_500_DEG:
            m_gyroScale = 65.5;
            break;
        case GYRO_RANGE_1000_DEG:
            m_gyroScale = 32.8;
            break;
        case GYRO_RANGE_2000_DEG:
            m_gyroScale = 16.4;
            break;
        default:
    }

    Wire.beginTransmission(m_addr);
    Wire.write(MPU6050_REG_GYRO_CONFIG);   
    Wire.write(range);       
    Wire.endTransmission(true);
}

uint8_t MPU6050::readByte(uint8_t reg){
    uint8_t data;

    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(true);

    Wire.requestFrom(reg, 1);

    if(Wire.available()){
        data = Wire.read();
    }

    return data;
}


void MPU6050::readBytes(uint8_t reg, uint8_t count, uint8_t* dest){
    Wire.beginTransmission(m_addr);
    Wire.write(reg);
    Wire.endTransmission(true);

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
    uint16_t x = raw_data[0] << 8 | raw_data[1];
    uint16_t y = raw_data[2] << 8 | raw_data[3];
    uint16_t z = raw_data[4] << 8 | raw_data[5];
    
    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readRawGyro(){
    // 3 Mesurements of 2 bytes each
    const int BYTES_TO_READ = 6;
    uint8_t raw_data[BYTES_TO_READ];

    readBytes(MPU6050_REG_GYRO_XOUT_H, BYTES_TO_READ, raw_data);
    uint16_t x = raw_data[0] << 8 | raw_data[1];
    uint16_t y = raw_data[2] << 8 | raw_data[3];
    uint16_t z = raw_data[4] << 8 | raw_data[5];
    
    Vector3D res = {(float)x, (float)y, (float)z};
    return res;
}

Vector3D MPU6050::readScaledAccel(){
    Vector3D raw = readRawAccel();
    return rawToScaled(raw.x, raw.y, raw.y, m_accelScale);
}

Vector3D MPU6050::readScaledGyro(){
    Vector3D raw = readRawGyro();
    return rawToScaled(raw.x, raw.y, raw.y, m_gyroScale);
}