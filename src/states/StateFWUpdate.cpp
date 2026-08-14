#include "states/StateFWUpdate.h"
#include "common/MyGlobal.h"

void StateFWUpdate::onEnter()
{
  Serial.println("Entering STATE_FW_UPDATE");
}

MyState StateFWUpdate::handleEvent(const MyEvent *event)
{
  switch (event->id)
  {
  case EVT_FW_UPDATING:
    return STATE_FW_UPDATING;
  case EVT_BLE_DISCONNECTED:
    return STATE_ADVERTISE;
  default:
    return STATE_FW_UPDATE;
  }
}

void StateFWUpdating::onEnter()
{
  Serial.println("Entering STATE_FW_UPDATING");
}

MyState StateFWUpdating::handleEvent(const MyEvent *event)
{
  switch (event->id)
  {
  case EVT_BLE_DISCONNECTED:
    return STATE_ADVERTISE;
  default:
    return STATE_FW_UPDATING;
  }
}
