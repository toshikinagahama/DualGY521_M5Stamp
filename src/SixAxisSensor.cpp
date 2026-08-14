#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>
#include "SixAxisSensor.h"

void SixAxisSensor::initialize()
{
  Wire.begin();
  this->sensor1 = new MPU6050(this->addr1);
  this->sensor2 = new MPU6050(this->addr2);
  this->sensor1->initialize();
  this->sensor2->initialize();

  // デフォルト設定の適用
  applySettings(this->accel_range, this->gyro_range, this->motion_threshold);
}

void SixAxisSensor::applySettings(uint8_t accRange, uint8_t gyroRange, uint8_t motionThresh)
{
  this->accel_range = (accRange <= 3) ? accRange : 0;
  this->gyro_range = (gyroRange <= 3) ? gyroRange : 0;
  this->motion_threshold = motionThresh;

  if (this->sensor1)
  {
    this->sensor1->setFullScaleAccelRange(this->accel_range);
    this->sensor1->setFullScaleGyroRange(this->gyro_range);
    this->sensor1->setMotionDetectionThreshold(this->motion_threshold);
  }
  if (this->sensor2)
  {
    this->sensor2->setFullScaleAccelRange(this->accel_range);
    this->sensor2->setFullScaleGyroRange(this->gyro_range);
    this->sensor2->setMotionDetectionThreshold(this->motion_threshold);
  }
}

void SixAxisSensor::getValues()
{
  this->sensor1->getMotion6(&this->raw_acc[0][0],
                            &this->raw_acc[0][1],
                            &this->raw_acc[0][2],
                            &this->raw_gyro[0][0],
                            &this->raw_gyro[0][1],
                            &this->raw_gyro[0][2]);
  this->sensor2->getMotion6(&this->raw_acc[1][0],
                            &this->raw_acc[1][1],
                            &this->raw_acc[1][2],
                            &this->raw_gyro[1][0],
                            &this->raw_gyro[1][1],
                            &this->raw_gyro[1][2]);
}
