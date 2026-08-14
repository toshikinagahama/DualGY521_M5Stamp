#include "states/StateAdvertise.h"
#include "common/MyGlobal.h"

void StateAdvertise::onEnter()
{
  Serial.println("Entering STATE_ADVERTISE");
  ble->advertiseStart();
}

MyState StateAdvertise::handleEvent(const MyEvent *event)
{
  switch (event->id)
  {
  case EVT_BLE_CONNECTED:
    Serial.println("BLE Connected -> Transition to STATE_WAIT");
    return STATE_WAIT;
  default:
    return STATE_ADVERTISE;
  }
}
