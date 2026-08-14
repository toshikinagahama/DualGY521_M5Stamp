#include "Arduino.h"
#include <FastLED.h>
#include <ArduinoOTA.h>
#include <Wire.h>

#include "common/MyGlobal.h"
#include "states/StateManager.h"
#include "states/StateAdvertise.h"
#include "states/StateWait.h"
#include "states/StateMeas.h"
#include "states/StateFWUpdate.h"

#define NUM_LEDS 1
#define DATA_PIN 27

static StateManager stateManager;
static uint16_t cnt = 0;

// 52バイトのBLE送信用構造体 (float 物理値: Acc=g, Gyro=dps)
struct __attribute__((packed)) SensorPacket {
  uint16_t header = 0x0101; // 識別子 (0x01, 0x01)
  uint16_t count;
  float    acc1[3];  // g
  float    gyro1[3]; // dps
  float    acc2[3];  // g
  float    gyro2[3]; // dps
};

/**
 * @brief データサンプリング & BLE送信 (float 物理値)
 */
void sampling()
{
  six_sensor->getValues();

  SensorPacket pkt;
  pkt.count = cnt;
  memcpy(pkt.acc1,  six_sensor->acc[0],  sizeof(pkt.acc1));
  memcpy(pkt.gyro1, six_sensor->gyro[0], sizeof(pkt.gyro1));
  memcpy(pkt.acc2,  six_sensor->acc[1],  sizeof(pkt.acc2));
  memcpy(pkt.gyro2, six_sensor->gyro[1], sizeof(pkt.gyro2));

  ble->notify((uint8_t *)&pkt, sizeof(pkt));
}

/**
 * @brief 【FreeRTOS Task 1】高精度センサ測定タスク (Priority 3: High)
 * vTaskDelayUntil による精密 100Hz 制御 (ビジーウェイト排除 & CPU負荷0%)
 */
void sensorTaskFunc(void *arg)
{
  TickType_t xLastWakeTime;
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xLastWakeTime = xTaskGetTickCount();

    while (stateManager.getCurrentStateID() == STATE_MEAS)
    {
      sampling();
      cnt++;
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
  }
}

/**
 * @brief 【FreeRTOS Task 2】独立バックグラウンド LED 点滅タスク (Priority 1: Low)
 */
void uiBlinkTaskFunc(void *arg)
{
  while (1)
  {
    switch (stateManager.getCurrentStateID())
    {
    case STATE_ADVERTISE:  ledController.blink(CRGB(0, 0, 255),   100, 900); break; // 未接続: 青1s点滅
    case STATE_WAIT:       ledController.blink(CRGB(0, 255, 0),   100, 900); break; // 接続待機: 緑1s点滅
    case STATE_MEAS:       ledController.blink(CRGB(255, 0, 0),   100, 400); break; // 測定中: 赤色高速点滅
    case STATE_FW_UPDATE:  ledController.blink(CRGB(255, 0, 255), 100, 900); break; // OTAモード: 紫点滅
    case STATE_FW_UPDATING:ledController.blink(CRGB(0, 255, 255), 500, 0);   break; // OTA書込中: 水色常時点灯
    default:               ledController.blink(CRGB(0, 0, 0),     100, 500); break;
    }
  }
}

/**
 * @brief 初期化関数
 */
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting DualGY521_M5Stamp");

  init_event_queue();
  init_eeprom_settings();

  // State Pattern 状態オブジェクトの登録 (静的インスタンス参照)
  static StateAdvertise sAdvertise;
  static StateWait      sWait;
  static StateMeas      sMeas;
  static StateFWUpdate  sFWUpdate;
  static StateFWUpdate  sFWUpdating; // 兼用
  stateManager.registerState(&sAdvertise);
  stateManager.registerState(&sWait);
  stateManager.registerState(&sMeas);
  stateManager.registerState(&sFWUpdate);

  ble->initialize();
  six_sensor->initialize();

  ledController.begin();

  // 初期状態の設定
  stateManager.changeState(STATE_ADVERTISE);

  // 【FreeRTOS タスクの作成】
  xTaskCreatePinnedToCore(sensorTaskFunc, "SensorTask", 4096, NULL, 3, &hSensorTask, 1);
  xTaskCreatePinnedToCore(uiBlinkTaskFunc, "UIBlinkTask", 2048, NULL, 1, NULL, 1);
}

/**
 * @brief 【Main Task / loop()】イベントディスパッチ＆OTAハンドリング
 */
void loop(void)
{
  ArduinoOTA.handle();

  MyState curState = stateManager.getCurrentStateID();
  TickType_t waitTicks = (curState == STATE_FW_UPDATE || curState == STATE_FW_UPDATING) ? pdMS_TO_TICKS(10) : portMAX_DELAY;

  MyEvent event = dequeue(waitTicks);

  if (event.id != EVT_NOP)
  {
    stateManager.handleEvent(&event);
  }
}