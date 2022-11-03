#include "Arduino.h"
#include "global.h"
#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
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
// const char *ssid = "aterm-2336f5-g";
// const char *password = "09991a8bae353";

/**
 *
 * イベント検知
 */
void DETECT_EVENT()
{
  if (!IsConnected)
    state = STATE_ADVERTISE;
  else if (IsMeasStop)
    if (!IsFirmwareUpdating)
    {
      state = STATE_WAIT_MEAS;
    }
    else
    {
      if (wifi_ssid.empty())
      {
        state = STATE_WAIT_MEAS;
      }
      else
      {
        state = STATE_FIRMWARE_UPDATING;
      }
    }
  else
    state = STATE_MEAS;
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
  Serial.println("Booting");
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED)
  // {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }
  // ArduinoOTA.setHostname("myesp32");

  // ArduinoOTA
  //     .onStart([]()
  //              {
  //     String type;
  //     if (ArduinoOTA.getCommand() == U_FLASH)
  //       type = "sketch";
  //     else // U_SPIFFS
  //       type = "filesystem";
  //     Serial.println("Start updating " + type); })
  //     .onEnd([]()
  //            { Serial.println("\nEnd"); })
  //     .onProgress([](unsigned int progress, unsigned int total)
  //                 { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
  //     .onError([](ota_error_t error)
  //              {
  //     Serial.printf("Error[%u]: ", error);
  //     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  //     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  //     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  //     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  //     else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  // ArduinoOTA.begin();

  // Serial.println("Ready");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
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
  // ArduinoOTA.handle();
  DETECT_EVENT();

  switch (state)
  {
  case STATE_ADVERTISE:
    IsFirmwareUpdating = false;
    leds[0] = 0x0000ff;
    FastLED.show();
    break;
  case STATE_WAIT_MEAS:
    leds[0] = 0x00ff00;
    FastLED.show();
    Serial.println("aaaa");
    if (!IsMeasStop)
      state = STATE_MEAS;
    break;
  case STATE_MEAS:
    leds[0] = 0xff0000;
    FastLED.show();
    if (IsMeasStop)
      state = STATE_WAIT_MEAS;
    break;
  case STATE_FIRMWARE_UPDATING:
    leds[0] = 0xff00ff;
    FastLED.show();
    if (!IsFirmwareUpdating)
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