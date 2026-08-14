#include "MyHandler_FW.h"

MyState handler_fw_nop(const MyEvent *event) {
  return STATE_FW_UPDATE;
}

MyState handler_fw_updating(const MyEvent *event) {
  return STATE_FW_UPDATING;
}

MyState handler_fw_ble_disconnected(const MyEvent *event) {
  return STATE_ADVERTISE;
}
