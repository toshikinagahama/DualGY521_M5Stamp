#include <Arduino.h>
#include <Wire.h>
#include "SixAxisSensor.h"

void SixAxisSensor::initialize()
{
  Wire.begin();
  Wire.beginTransmission(this->addr1);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(100);
  Wire.beginTransmission(this->addr2);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void SixAxisSensor::getValues()
{
  // Sensor1
  Wire.beginTransmission(this->addr1);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(this->addr1, 14, true);               // request a total of 14 registers
  this->raw_acc[0][0] = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  this->raw_acc[0][1] = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  this->raw_acc[0][2] = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  this->raw_temp[0] = Wire.read() << 8 | Wire.read();    // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  this->raw_gyro[0][0] = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  this->raw_gyro[0][1] = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  this->raw_gyro[0][2] = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  // Sensor2
  Wire.beginTransmission(this->addr2);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(this->addr2, 14, true);               // request a total of 14 registers
  this->raw_acc[1][0] = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  this->raw_acc[1][1] = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  this->raw_acc[1][2] = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  this->raw_temp[1] = Wire.read() << 8 | Wire.read();    // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  this->raw_gyro[1][0] = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  this->raw_gyro[1][1] = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  this->raw_gyro[1][2] = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
