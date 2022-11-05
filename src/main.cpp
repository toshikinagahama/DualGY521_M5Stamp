#include "Arduino.h"
#include "EEPROM.h"
#include "global.h"
#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "MyBLE.h"
#include "event.h"
#include "BatterySensor.h"
#include "SixAxisSensor.h"
#define NUM_LEDS 1
#define DATA_PIN 27

CRGB leds[NUM_LEDS];

MyBLE *ble = new MyBLE();
SixAxisSensor *six_sensor = new SixAxisSensor();
BatterySensor *bat_sensor = new BatterySensor();
MyState state;
uint16_t cnt = 0;

int sampling_period_us;

/**
 * @brief データサンプリング
 *
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
  // sprintf(val, "data,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", cnt, six_sensor->raw_acc[0][0], six_sensor->raw_acc[0][1], six_sensor->raw_acc[0][2], six_sensor->raw_gyro[0][0], six_sensor->raw_gyro[0][1], six_sensor->raw_gyro[0][2], six_sensor->raw_acc[1][0], six_sensor->raw_acc[1][1], six_sensor->raw_acc[1][2], six_sensor->raw_gyro[1][0], six_sensor->raw_gyro[1][1], six_sensor->raw_gyro[1][2]);
  uint16_t t = cnt;
  ble->notify(val, 28);
  // ble->notify(val);
  //サンプリング分待つ
  unsigned long time_end = micros();
  while (time_end - time_start < sampling_period_us)
  {
    time_end = micros();
  }
}

/**
 * @brief 非同期のタスク
 *
 * @param arg
 */
void task(void *arg)
{
  char val[128];
  while (1)
  {
    switch (state)
    {
    case STATE_MEAS:
      sampling();
      cnt++;
      break;
    default:
      bat_sensor->getValues();
      val[128];
      sprintf(val, "%d", bat_sensor->level);
      ble->pBatteryCharacteristic->setValue(&bat_sensor->level, 1);
      ble->pBatteryCharacteristic->notify();
      delay(1000);
      break;
    }
  }
}

/**
 * @brief Wifiアクセスポイントに接続
 *
 */
void connectWifiAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SYS.WIFI_SSID.c_str(), SYS.WIFI_PW.c_str());
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    // ble->notify("Info,Connection Failed! Rebooting...");
    //  Serial.println("Connection Failed! Rebooting...");
    delay(3000);
    ESP.restart();
  }
  SYS.WIFI_IP = WiFi.localIP().toString().c_str();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * @brief ファームウェア更新
 *
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
  INT.BIT.FW_UPDATING = 1;
}

/**
 * @brief 初期化関数
 *
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  if (!EEPROM.begin(1000))
  {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  EEPROM.writeInt(ADDRESS_MAJOR_VERSION, 0);
  EEPROM.writeInt(ADDRESS_MINOR_VERSION, 2);
  EEPROM.writeInt(ADDRESS_REVISION_VERSION, 1);
  EEPROM.commit();
  char char_version[64];
  sprintf(char_version, "%d.%d.%d", EEPROM.readByte(ADDRESS_MAJOR_VERSION), EEPROM.readByte(ADDRESS_MINOR_VERSION), EEPROM.readByte(ADDRESS_REVISION_VERSION));
  SYS.DEVICE_VERSION = char_version;
  Serial.println(SYS.DEVICE_VERSION.c_str());
  ble->initialize();
  bat_sensor->initialize();
  six_sensor->initialize();
  FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS); // GRB ordering is typical
  ble->advertiseStart();
  sampling_period_us = round(1000000 * (1.0 / 100)); //サンプリング時間の設定
  xTaskCreatePinnedToCore(task, "task", 4096, NULL, 1, NULL, 1);
  state = STATE_ADVERTISE;
}

/**
 * @brief アクション実行
 *
 * @param EVT
 */
void doAction(MyEvent EVT)
{
  switch (state)
  {
  case STATE_ADVERTISE:
    switch (EVT)
    {
    case EVT_BLE_CONNECTED:
      state = STATE_WAIT;
      break;
    default:
      break;
    }
    leds[0] = 0x0000ff;
    FastLED.show();
    break;
  case STATE_WAIT:
    switch (EVT)
    {
    case EVT_BLE_DISCONNECTED:
      state = STATE_ADVERTISE;
      break;
    case EVT_MEAS_START:
      cnt = 0;
      state = STATE_MEAS;
      break;
    case EVT_CONNECT_WIFI:
      connectWifiAP(); //アクセスポイントに接続
      break;
    case EVT_FW_UPDATE:
      firmwareUpdate(); //ファームウェア更新
      state = STATE_FW_UPDATE;
      break;
    case EVT_GET_DEVICE_INFO:
      //本当は設定状態に分けたほうがいい
      ble->notify("device_version," + SYS.DEVICE_VERSION);
      break;
    case EVT_GET_WIFI_SSID:
      ble->notify("wifi_ssid," + SYS.WIFI_SSID);
      break;
    case EVT_SET_WIFI_SSID:
      SYS.WIFI_SSID = SYS.BLE_ARG; //引数に入ってる
      SYS.BLE_ARG = "";
      ble->notify("wifi_ssid," + SYS.WIFI_SSID);
      break;
    case EVT_GET_WIFI_PW:
      ble->notify("wifi_pw," + SYS.WIFI_PW);
      break;
    case EVT_SET_WIFI_PW:
      SYS.WIFI_PW = SYS.BLE_ARG; //引数に入ってる
      SYS.BLE_ARG = "";
      ble->notify("wifi_pw," + SYS.WIFI_PW);
      break;
    case EVT_GET_WIFI_IP:
      ble->notify("device_ip," + SYS.WIFI_IP);
      break;
    case EVT_GET_FW_HOST:
      ble->notify("fw_host," + SYS.FW_HOST);
      break;
    case EVT_SET_FW_HOST:
      SYS.FW_HOST = SYS.BLE_ARG; //引数に入ってる
      SYS.BLE_ARG = "";
      ble->notify("fw_host," + SYS.FW_HOST);
    default:
      break;
    }
    leds[0] = 0xff0000;
    FastLED.show();
    break;
  case STATE_MEAS:
    switch (EVT)
    {
    case EVT_MEAS_STOP:
      state = STATE_WAIT;
      break;
    case EVT_BLE_DISCONNECTED:
      state = STATE_ADVERTISE;
      break;
    default:
      break;
    }
    leds[0] = 0x00ff00;
    FastLED.show();
    break;
  case STATE_FW_UPDATE:
    switch (EVT)
    {
    case EVT_FW_UPDATING:
      state = STATE_FW_UPDATING;
      break;
    case EVT_BLE_DISCONNECTED:
      state = STATE_ADVERTISE;
      break;
    default:
      break;
    }
    leds[0] = 0xff00ff;
    FastLED.show();
    break;
  case STATE_FW_UPDATING:
    switch (EVT)
    {
    case EVT_BLE_DISCONNECTED:
      state = STATE_ADVERTISE;
      break;
    default:
      break;
    }
    leds[0] = 0x000000;
    FastLED.show();
    delay(200);
    leds[0] = 0x00ffff;
    FastLED.show();
    break;
  default:
    leds[0] = 0xffffff;
    FastLED.show();
    break;
  }
  delay(10);
}

/**
 * @brief ループ関数
 *
 */
void loop(void)
{
  ArduinoOTA.handle();          //ファームウェア更新のハンドラ
  MyEvent EVT = Detect_Event(); //イベント検知
  doAction(EVT);                //イベントに応じたアクション実行
  Release_Event(EVT);           //イベント開放
}