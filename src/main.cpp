#include "Arduino.h"
#include "global.h"
#include <FastLED.h>
#include <Wire.h>
#include "MyBLE.h"
#include "BatterySensor.h"
#include "SixAxisSensor.h"

#define NUM_LEDS 1
#define DATA_PIN 27
CRGB leds[NUM_LEDS];

MyBLE *ble = new MyBLE();
SixAxisSensor *six_sensor = new SixAxisSensor();
BatterySensor *bat_sensor = new BatterySensor();

int sampling_period_us;

/**
 *
 * イベント検知
 */
void DETECT_EVENT()
{
  if (!IsConnected)
  {
    state = STATE_ADVERTISE;
  }
  else
  {
    if (IsMeasStop)
    {
      state = STATE_WAIT_MEAS;
    }
    else
    {
      state = STATE_MEAS;
    }
  }
}

void sampling(void *arg)
{
  while (1)
  {

    if (state == STATE_MEAS)
    {
      if (IsConnected)
      {
        if (!IsMeasStop)
        {
          unsigned long time_start = micros();
          six_sensor->getValues();
          char val[128];
          sprintf(val, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", six_sensor->raw_acc[0][0], six_sensor->raw_acc[0][1], six_sensor->raw_acc[0][2], six_sensor->raw_gyro[0][0], six_sensor->raw_gyro[0][1], six_sensor->raw_gyro[0][2], six_sensor->raw_acc[1][0], six_sensor->raw_acc[1][1], six_sensor->raw_acc[1][2], six_sensor->raw_gyro[1][0], six_sensor->raw_gyro[1][1], six_sensor->raw_gyro[1][2]);
          ble->notify(val);
          // Serial.printf(val);
          //サンプリング分待つ
          unsigned long time_end = micros();
          while (time_end - time_start < sampling_period_us)
          {
            time_end = micros();
          }
        }
        else
        {
          delay(1000);
        }
      }
      else
      {
        delay(1000);
      }
    }
    else
    {
      delay(1000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  ble->initialize();
  bat_sensor->initialize();
  six_sensor->initialize();
  FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS); // GRB ordering is typical
  ble->advertiseStart();
  sampling_period_us = round(1000000 * (1.0 / 100)); //サンプリング時間の設定
  xTaskCreatePinnedToCore(sampling, "sampling", 4096, NULL, 1, NULL, 1);
  state = STATE_ADVERTISE;
}

void loop(void)
{
  if (IsConnected)
  {
    state = STATE_MEAS;
  }
  DETECT_EVENT();

  switch (state)
  {
  case STATE_ADVERTISE:
    leds[0] = 0x0000ff;
    FastLED.show();
    break;
  case STATE_WAIT_MEAS:
    leds[0] = 0x00ff00;
    FastLED.show();
    if (!IsMeasStop)
      state = STATE_MEAS;
    break;
  case STATE_MEAS:
    leds[0] = 0xff0000;
    FastLED.show();
    if (IsMeasStop)
      state = STATE_WAIT_MEAS;
    break;
  default:
    leds[0] = 0xffffff;
    FastLED.show();
    break;
  }
  // Now turn the LED off, then pause
  delay(500);
  bat_sensor->getValues();
  char val[128];
  sprintf(val, "%d", bat_sensor->level);
  ble->pBatteryCharacteristic->setValue(&bat_sensor->level, 1);
  ble->pBatteryCharacteristic->notify();
  // ble->notify(val);
}