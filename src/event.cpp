#include "event.h"
#include "global.h"

/**
 *
 * イベント検知
 */
MyEvent Detect_Event()
{
  if (INT.BIT.BLE_CONNECTED == 1)
    return EVT_BLE_CONNECTED;
  if (INT.BIT.BLE_DISCONNECTED == 1)
    return EVT_BLE_DISCONNECTED;
  if (INT.BIT.CMD_MEAS_START == 1)
    return EVT_MEAS_START;
  if (INT.BIT.CMD_MEAS_STOP == 1)
    return EVT_MEAS_STOP;
  if (INT.BIT.CMD_FW_UPDATE == 1)
  {
    if (!SYS.WIFI_SSID.empty())
      return EVT_FW_UPDATE;
    else
      return EVT_NOP;
  }
  if (INT.BIT.FW_UPDATE == 1)
    return EVT_FW_UPDATING;
  if (INT.BIT.CMD_GET_DEVICE_INFO == 1)
    return EVT_GET_DEVICE_INFO;
  if (INT.BIT.CMD_SET_WIFI_SSID == 1)
    return EVT_SET_WIFI_SSID;
  if (INT.BIT.CMD_GET_WIFI_SSID == 1)
    return EVT_GET_WIFI_SSID;
  if (INT.BIT.CMD_SET_WIFI_PW == 1)
    return EVT_SET_WIFI_PW;
  if (INT.BIT.CMD_GET_WIFI_PW == 1)
    return EVT_GET_WIFI_PW;
  if (INT.BIT.CMD_GET_WIFI_IP == 1)
    return EVT_GET_WIFI_IP;
  if (INT.BIT.CMD_CONNECT_WIFI == 1)
    return EVT_CONNECT_WIFI;
  if (INT.BIT.WIFI_CONNECTED == 1)
    return EVT_WIFI_CONNECTED;
  if (INT.BIT.WIFI_DISCONNECTED == 1)
    return EVT_WIFI_CONNECTED;
  if (INT.BIT.CMD_SET_FW_HOST == 1)
    return EVT_SET_FW_HOST;
  if (INT.BIT.CMD_GET_FW_HOST == 1)
    return EVT_GET_FW_HOST;
  if (INT.BIT.FW_UPDATING == 1)
    return EVT_FW_UPDATING;
  return EVT_NOP;
}

/**
 * @brief イベント開放
 *
 */
void Release_Event(MyEvent EVT)
{
  switch (EVT)
  {
  case EVT_BLE_CONNECTED:
    INT.BIT.BLE_CONNECTED = 0;
    break;
  case EVT_BLE_DISCONNECTED:
    INT.BIT.BLE_DISCONNECTED = 0;
    break;
  case EVT_MEAS_START:
    INT.BIT.CMD_MEAS_START = 0;
    break;
  case EVT_MEAS_STOP:
    INT.BIT.CMD_MEAS_STOP = 0;
    break;
  case EVT_GET_DEVICE_INFO:
    INT.BIT.CMD_GET_DEVICE_INFO = 0;
    break;
  case EVT_SET_WIFI_SSID:
    INT.BIT.CMD_SET_WIFI_SSID = 0;
    break;
  case EVT_GET_WIFI_SSID:
    INT.BIT.CMD_GET_WIFI_SSID = 0;
    break;
  case EVT_SET_WIFI_PW:
    INT.BIT.CMD_SET_WIFI_PW = 0;
    break;
  case EVT_GET_WIFI_PW:
    INT.BIT.CMD_GET_WIFI_PW = 0;
    break;
  case EVT_GET_WIFI_IP:
    INT.BIT.CMD_GET_WIFI_IP = 0;
    break;
  case EVT_SET_FW_HOST:
    INT.BIT.CMD_SET_FW_HOST = 0;
    break;
  case EVT_GET_FW_HOST:
    INT.BIT.CMD_GET_FW_HOST = 0;
    break;
  case EVT_CONNECT_WIFI:
    INT.BIT.CMD_CONNECT_WIFI = 0;
    break;
  case EVT_WIFI_CONNECTED:
    INT.BIT.WIFI_CONNECTED = 0;
    break;
  case EVT_WIFI_DISCONNECTED:
    INT.BIT.WIFI_DISCONNECTED = 0;
    break;
  case EVT_FW_UPDATE:
    INT.BIT.CMD_FW_UPDATE = 0;
    break;
  case EVT_FW_UPDATING:
    INT.BIT.FW_UPDATING = 0;
    break;
  default:
    break;
  }
}