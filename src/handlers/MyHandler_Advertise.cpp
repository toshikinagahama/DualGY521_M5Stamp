#include "MyHandler_Advertise.h"

MyState handler_adv_nop(const MyEvent *event) {
  return STATE_ADVERTISE;
}

MyState handler_adv_ble_connected(const MyEvent *event) {
  Serial.println("State -> STATE_WAIT");
  return STATE_WAIT;
}

MyState handler_adv_ble_disconnected(const MyEvent *event) {
  return STATE_ADVERTISE;
}
