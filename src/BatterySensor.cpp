#include "BatterySensor.h"

void BatterySensor::initialize()
{
  pinMode(this->pin_wakeup, OUTPUT);
  pinMode(this->pin_read, INPUT);
}

void BatterySensor::getValues()
{
  digitalWrite(this->pin_wakeup, HIGH); // トランジスタON
  delay(100);
  this->raw_vol = analogRead(this->pin_read);
  float vol = (float)this->raw_vol * 2 * (1.85 / 4095);
  this->level = int((vol - 1.441138) / (2.24415 - 1.441138) * 100);
  // Serial.printf("%d\n", this->level);
  digitalWrite(this->pin_wakeup, LOW); // トランジスタOFF
}