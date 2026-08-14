#include "common/LedController.h"

LedController ledController(27, 20);

LedController::LedController(uint8_t pin, uint8_t defaultBrightness)
    : dataPin(pin), brightness(defaultBrightness)
{
}

void LedController::begin()
{
  FastLED.addLeds<SK6812, 27, GRB>(leds, 1);
  FastLED.setBrightness(brightness);
  clear();
}

void LedController::setBrightness(uint8_t b)
{
  brightness = b;
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void LedController::setColor(CRGB color)
{
  leds[0] = color;
  FastLED.show();
}

void LedController::clear()
{
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
}

void LedController::blink(CRGB color, uint32_t onMs, uint32_t offMs)
{
  setColor(color);
  vTaskDelay(pdMS_TO_TICKS(onMs));

  if (offMs > 0)
  {
    clear();
    vTaskDelay(pdMS_TO_TICKS(offMs));
  }
}
