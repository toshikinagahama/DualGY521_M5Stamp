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
  // set full scale range
  this->sensor1->setFullScaleAccelRange(2);
  this->sensor1->setFullScaleGyroRange(2);
  this->sensor2->setFullScaleAccelRange(2);
  this->sensor2->setFullScaleGyroRange(2);
  // caribrate
  // this->sensor1->CalibrateAccel();
  // this->sensor1->CalibrateGyro();
  // this->sensor2->CalibrateAccel();
  // this->sensor2->CalibrateGyro();
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
