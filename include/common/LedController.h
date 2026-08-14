#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <Arduino.h>
#include <FastLED.h>

/**
 * @brief SK6812 内蔵 RGB LED コントローラ
 */
class LedController
{
private:
  CRGB leds[1];
  uint8_t dataPin;
  uint8_t brightness;

public:
  LedController(uint8_t pin = 27, uint8_t defaultBrightness = 20);

  /**
   * @brief FastLEDの初期化
   */
  void begin();

  /**
   * @brief LED輝度設定
   */
  void setBrightness(uint8_t b);

  /**
   * @brief LEDを指定色で点灯
   */
  void setColor(CRGB color);

  /**
   * @brief LED消灯
   */
  void clear();

  /**
   * @brief 指定色で点滅 (ノンブロッキング FreeRTOS Delay)
   */
  void blink(CRGB color, uint32_t onMs, uint32_t offMs);
};

extern LedController ledController;

#endif // LEDCONTROLLER_H
