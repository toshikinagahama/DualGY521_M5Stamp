#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>
#include "SixAxisSensor.h"

void SixAxisSensor::initialize()
{
  Wire.begin();
  Wire.setClock(400000); // I2C Fast Mode (400kHz) 通信時間を約3ms→0.8msに高速化
  this->sensor1.initialize();
  this->sensor2.initialize();

  // デフォルト設定の適用
  applySettings(this->accel_range, this->gyro_range);
}

void SixAxisSensor::applySettings(uint8_t accRange, uint8_t gyroRange)
{
  this->accel_range = (accRange <= 3) ? accRange : 0;
  this->gyro_range = (gyroRange <= 3) ? gyroRange : 0;

  this->sensor1.setFullScaleAccelRange(this->accel_range);
  this->sensor1.setFullScaleGyroRange(this->gyro_range);
  this->sensor2.setFullScaleAccelRange(this->accel_range);
  this->sensor2.setFullScaleGyroRange(this->gyro_range);
}

void SixAxisSensor::getValues()
{
  this->sensor1.getMotion6(&this->raw_acc[0][0],
                           &this->raw_acc[0][1],
                           &this->raw_acc[0][2],
                           &this->raw_gyro[0][0],
                           &this->raw_gyro[0][1],
                           &this->raw_gyro[0][2]);
  this->sensor2.getMotion6(&this->raw_acc[1][0],
                           &this->raw_acc[1][1],
                           &this->raw_acc[1][2],
                           &this->raw_gyro[1][0],
                           &this->raw_gyro[1][1],
                           &this->raw_gyro[1][2]);

  // LSB スケールルックアップテーブル (0: ±2g/±250dps, 1: ±4g/±500dps, 2: ±8g/±1000dps, 3: ±16g/±2000dps)
  static const float ACC_SCALES[4]  = {16384.0f, 8192.0f, 4096.0f, 2048.0f};
  static const float GYRO_SCALES[4] = {131.0f, 65.5f, 32.8f, 16.4f};

  float accScale  = ACC_SCALES[this->accel_range & 0x03];
  float gyroScale = GYRO_SCALES[this->gyro_range & 0x03];

  // float 物理値への変換 (Acc: g, Gyro: dps) および オフセット補正 (NaNガード付き)
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      float offAcc = isnan(this->acc_offset[i][j]) ? 0.0f : this->acc_offset[i][j];
      float offGyro = isnan(this->gyro_offset[i][j]) ? 0.0f : this->gyro_offset[i][j];

      this->acc[i][j] = ((float)this->raw_acc[i][j] / accScale) - offAcc;
      this->gyro[i][j] = ((float)this->raw_gyro[i][j] / gyroScale) - offGyro;

      if (isnan(this->acc[i][j])) this->acc[i][j] = 0.0f;
      if (isnan(this->gyro[i][j])) this->gyro[i][j] = 0.0f;
    }
  }
}

/**
 * @brief 静止状態で100サンプル測定しゼロ点オフセットをキャリブレーション
 */
void SixAxisSensor::calibrate()
{
  float sumAcc[2][3] = {0};
  float sumGyro[2][3] = {0};
  const int SAMPLES = 100;

  // キャリブレーション中のオフセット補正を一時無効化
  memset(this->acc_offset, 0, sizeof(this->acc_offset));
  memset(this->gyro_offset, 0, sizeof(this->gyro_offset));

  for (int s = 0; s < SAMPLES; s++)
  {
    getValues();
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        sumAcc[i][j] += this->acc[i][j];
        sumGyro[i][j] += this->gyro[i][j];
      }
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->acc_offset[i][j] = sumAcc[i][j] / (float)SAMPLES;
      this->gyro_offset[i][j] = sumGyro[i][j] / (float)SAMPLES;
    }
    // Z軸重力加速度 1.0g の補正（静止時に Z軸が 1g になるよう期待値を差し引く）
    this->acc_offset[i][2] -= 1.0f;
  }
}
