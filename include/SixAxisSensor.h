#ifndef SIXAXISSENSOR_H
#define SIXAXISSENSOR_H
#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//姿勢クラス
class SixAxisSensor
{

private:
  uint16_t addr1 = 0x68;
  uint16_t addr2 = 0x69;

public:
  int16_t raw_acc[2][3];
  int16_t raw_gyro[2][3];
  int16_t raw_temp[2] = {0};
  MPU6050 *sensor1;
  MPU6050 *sensor2;

  /**
   *
   * 初期化
   *
   */
  void initialize();
  void getValues();
};

#endif // SIXAXISSENSOR_H