#ifndef BATTERYSENSOR_H
#define BATTERYSENSOR_H
#include <Arduino.h>
class BatterySensor
{

private:
  uint8_t pin_wakeup = 26;
  uint8_t pin_read = 36;

public:
  int16_t raw_vol = 0;
  uint8_t level = 0;

  /**
   *
   * 初期化
   *
   */
  void initialize();
  void getValues();
};

#endif // BATTERYSENSOR_H