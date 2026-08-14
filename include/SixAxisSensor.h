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
  float acc[2][3] = {0};  // 物理値 (g)
  float gyro[2][3] = {0}; // 物理値 (dps)
  float acc_offset[2][3] = {0};  // オフセット (g, 静止時 1G Z補正含む)
  float gyro_offset[2][3] = {0}; // オフセット (dps)
  int16_t raw_temp[2] = {0};
  MPU6050 sensor1{0x68};
  MPU6050 sensor2{0x69};

  uint8_t accel_range = 0;      // 0: ±2g, 1: ±4g, 2: ±8g, 3: ±16g
  uint8_t gyro_range = 0;       // 0: ±250dps, 1: ±500dps, 2: ±1000dps, 3: ±2000dps

  /**
   * 初期化
   */
  void initialize();
  void getValues();
  void applySettings(uint8_t accRange, uint8_t gyroRange);
  void calibrate();
};

#endif // SIXAXISSENSOR_H