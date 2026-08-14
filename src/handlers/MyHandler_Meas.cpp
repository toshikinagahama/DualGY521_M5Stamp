#include "MyHandler_Meas.h"

MyState handler_meas_nop(const MyEvent *event) {
  return STATE_MEAS;
}

MyState handler_meas_cmd_meas_stop(const MyEvent *event) {
  Serial.println("State -> STATE_WAIT");
  return STATE_WAIT;
}

MyState handler_meas_ble_disconnected(const MyEvent *event) {
  Serial.println("State -> STATE_ADVERTISE");
  return STATE_ADVERTISE;
}
