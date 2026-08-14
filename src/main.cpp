#include "Arduino.h"
#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "common/MyGlobal.h"
#include "handlers/MyHandler.h"

#define NUM_LEDS 1
#define DATA_PIN 27

uint16_t cnt = 0;
int sampling_period_us;

/**
 * @brief データサンプリング
 */
void sampling()
{
  unsigned long time_start = micros();
  six_sensor->getValues();
  uint8_t val[28];
  val[0] = 0x01; //識別子1
  val[1] = 0x01; //識別子2
  val[2] = (uint8_t)(cnt);
  val[3] = (uint8_t)(cnt >> 8);
  val[4] = (uint8_t)(six_sensor->raw_acc[0][0]);
  val[5] = (uint8_t)(six_sensor->raw_acc[0][0] >> 8);
  val[6] = (uint8_t)(six_sensor->raw_acc[0][1]);
  val[7] = (uint8_t)(six_sensor->raw_acc[0][1] >> 8);
  val[8] = (uint8_t)(six_sensor->raw_acc[0][2]);
  val[9] = (uint8_t)(six_sensor->raw_acc[0][2] >> 8);
  val[10] = (uint8_t)(six_sensor->raw_gyro[0][0]);
  val[11] = (uint8_t)(six_sensor->raw_gyro[0][0] >> 8);
  val[12] = (uint8_t)(six_sensor->raw_gyro[0][1]);
  val[13] = (uint8_t)(six_sensor->raw_gyro[0][1] >> 8);
  val[14] = (uint8_t)(six_sensor->raw_gyro[0][2]);
  val[15] = (uint8_t)(six_sensor->raw_gyro[0][2] >> 8);
  val[16] = (uint8_t)(six_sensor->raw_acc[1][0]);
  val[17] = (uint8_t)(six_sensor->raw_acc[1][0] >> 8);
  val[18] = (uint8_t)(six_sensor->raw_acc[1][1]);
  val[19] = (uint8_t)(six_sensor->raw_acc[1][1] >> 8);
  val[20] = (uint8_t)(six_sensor->raw_acc[1][2]);
  val[21] = (uint8_t)(six_sensor->raw_acc[1][2] >> 8);
  val[22] = (uint8_t)(six_sensor->raw_gyro[1][0]);
  val[23] = (uint8_t)(six_sensor->raw_gyro[1][0] >> 8);
  val[24] = (uint8_t)(six_sensor->raw_gyro[1][1]);
  val[25] = (uint8_t)(six_sensor->raw_gyro[1][1] >> 8);
  val[26] = (uint8_t)(six_sensor->raw_gyro[1][2]);
  val[27] = (uint8_t)(six_sensor->raw_gyro[1][2] >> 8);
  ble->notify(val, 28);

  unsigned long time_end = micros();
  while (time_end - time_start < sampling_period_us)
  {
    time_end = micros();
  }
}

/**
 * @brief 非同期のタスク
 */
void task(void *arg)
{
  while (1)
  {
    switch (state)
    {
    case STATE_MEAS:
      sampling();
      cnt++;
      break;
    default:
      delay(100);
      break;
    }
  }
}

/**
 * @brief Wifiアクセスポイントに接続
 */
void connectWifiAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(sys.WIFI_SSID.c_str(), sys.WIFI_PW.c_str());
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(3000);
    ESP.restart();
  }
  sys.WIFI_IP = WiFi.localIP().toString().c_str();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * @brief ファームウェア更新
 */
void firmwareUpdate()
{
  ArduinoOTA.setHostname("myesp32");
  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
  enqueue(EVT_FW_UPDATING);
}

/**
 * @brief UI・LED更新 (省電力点滅制御)
 */
void ui_update()
{
  unsigned long now = millis();
  bool blink1s = (now % 1000) < 100; // 1秒周期 (100ms点灯 / 900ms消灯)

  switch (state)
  {
  case STATE_ADVERTISE:
    // 未接続: 青色 1秒周期点滅
    leds[0] = blink1s ? CRGB(0, 0, 255) : CRGB(0, 0, 0);
    FastLED.show();
    break;
  case STATE_WAIT:
    // 接続・待機: 緑色 1秒周期点滅
    leds[0] = blink1s ? CRGB(0, 255, 0) : CRGB(0, 0, 0);
    FastLED.show();
    break;
  case STATE_MEAS:
    // 接続・測定中: 緑色 高速点滅 (500ms周期)
    {
      bool blinkFast = (now % 500) < 100;
      leds[0] = blinkFast ? CRGB(0, 255, 0) : CRGB(0, 0, 0);
      FastLED.show();
    }
    break;
  case STATE_FW_UPDATE:
    // OTAモード: 紫色点滅
    leds[0] = blink1s ? CRGB(255, 0, 255) : CRGB(0, 0, 0);
    FastLED.show();
    break;
  case STATE_FW_UPDATING:
    // OTA書き込み中: 水色高速点滅
    {
      bool blinkFw = (now % 300) < 150;
      leds[0] = blinkFw ? CRGB(0, 255, 255) : CRGB(0, 0, 0);
      FastLED.show();
    }
    break;
  default:
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    break;
  }
}

/**
 * @brief 初期化関数
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  init_event_queue();
  init_eeprom_settings();
  init_state_handler_table();

  ble->initialize();
  six_sensor->initialize();
  FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(20); // 明るさを抑える (0~255)
  ble->advertiseStart();
  sampling_period_us = round(1000000 * (1.0 / 100));

  xTaskCreatePinnedToCore(task, "task", 4096, NULL, 1, NULL, 1);
  state = STATE_ADVERTISE;
  ui_update();
}

/**
 * @brief ループ関数
 */
void loop(void)
{
  ArduinoOTA.handle();

  // 点滅制御のため 50ms タイムアウトでキュー周回
  TickType_t waitTicks = (state == STATE_FW_UPDATE || state == STATE_FW_UPDATING) ? pdMS_TO_TICKS(10) : pdMS_TO_TICKS(50);
  MyEvent event = dequeue(waitTicks);

  if (state < STATE_MAX && event.id < EVT_MAX)
  {
    EventHandler handler = state_handler_table[state][event.id];
    if (handler != NULL)
    {
      state = handler(&event);
    }
  }

  ui_update();
}