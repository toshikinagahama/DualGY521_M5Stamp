#include "states/StateMeas.h"
#include "common/MyGlobal.h"

void StateMeas::onEnter()
{
  Serial.println("Entering STATE_MEAS -> Notifying SensorTask to start");
  if (hSensorTask != NULL)
  {
    xTaskNotifyGive(hSensorTask);
  }
}

void StateMeas::onExit()
{
  Serial.println("Exiting STATE_MEAS");
}

MyState StateMeas::handleEvent(const MyEvent *event)
{
  switch (event->id)
  {
  case EVT_CMD_MEAS_STOP:
    return STATE_WAIT;
  case EVT_BLE_DISCONNECTED:
    return STATE_ADVERTISE;
  default:
    return STATE_MEAS;
  }
}
