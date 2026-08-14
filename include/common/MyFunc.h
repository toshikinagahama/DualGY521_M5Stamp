#ifndef MYFUNC_H
#define MYFUNC_H

#include <Arduino.h>
#include <FastLED.h>
#include "LedController.h"

/**
 * @brief LEDを指定色で点滅 (ノンブロッキングFreeRTOS Delay)
 */
void blinkLed(CRGB color, uint32_t onMs, uint32_t offMs);

/**
 * @brief Wi-Fi アクセスポイントに接続
 */
void connectWifiAP();

/**
 * @brief ArduinoOTA ファームウェア更新の開始
 */
void firmwareUpdate();

#endif // MYFUNC_H
