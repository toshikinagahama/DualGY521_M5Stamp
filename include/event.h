#ifndef EVENT_H

#ifdef EVENT_H
#define EXTERN extern
#else
#define EXTERN
#endif
#define EVENT_H

#include <Arduino.h>

enum MyState
{
  STATE_WAIT,
  STATE_ADVERTISE,
  STATE_MEAS,
  STATE_FW_UPDATE,
  STATE_FW_UPDATING,
  // STATE_SETTING, 設定はWAIT状態で行うこととするので状態に入れない
  STATE_MAX,
};

enum MyEvent
{
  EVT_NOP,
  EVT_BLE_CONNECTED,
  EVT_BLE_DISCONNECTED,
  EVT_MEAS_START,
  EVT_MEAS_STOP,
  EVT_GET_DEVICE_INFO,
  EVT_SET_WIFI_SSID,
  EVT_GET_WIFI_SSID,
  EVT_SET_WIFI_PW,
  EVT_GET_WIFI_PW,
  EVT_GET_WIFI_IP,
  EVT_SET_FW_HOST,
  EVT_GET_FW_HOST,
  EVT_CONNECT_WIFI,
  EVT_WIFI_CONNECTED,
  EVT_WIFI_DISCONNECTED,
  EVT_FW_UPDATE,
  EVT_FW_UPDATING,
};

//外部宣言
extern MyEvent Detect_Event();
extern void Release_Event(MyEvent);
#endif // EVENT_H