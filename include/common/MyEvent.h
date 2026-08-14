#ifndef MYEVENT_H
#define MYEVENT_H

#include <Arduino.h>

#define PAYLOAD_SIZE 64

// BLE コマンドコード定義 (constexpr)
namespace BLECmd {
  constexpr uint16_t GET_DEVICE_INFO = 0x0001;
  constexpr uint16_t MEAS_START      = 0x0100;
  constexpr uint16_t MEAS_STOP       = 0x0101;
  constexpr uint16_t SET_WIFI_SSID   = 0x0200;
  constexpr uint16_t SET_WIFI_PW     = 0x0201;
  constexpr uint16_t GET_WIFI_IP     = 0x0202;
  constexpr uint16_t CONNECT_WIFI    = 0x0203;
  constexpr uint16_t FW_UPDATE       = 0x0300;
  constexpr uint16_t SET_FW_HOST     = 0x0301;
  constexpr uint16_t SET_SENSOR_CFG  = 0x0400;
  constexpr uint16_t GET_SENSOR_CFG  = 0x0401;
  constexpr uint16_t CALIBRATE       = 0x0402;
}

enum MyEventID {
  EVT_NOP = 0,
  EVT_BLE_CONNECTED,
  EVT_BLE_DISCONNECTED,
  EVT_CMD_MEAS_START,
  EVT_CMD_MEAS_STOP,
  EVT_CMD_GET_DEVICE_INFO,
  EVT_CMD_SET_WIFI_SSID,
  EVT_CMD_GET_WIFI_SSID,
  EVT_CMD_SET_WIFI_PW,
  EVT_CMD_GET_WIFI_PW,
  EVT_CMD_GET_WIFI_IP,
  EVT_CMD_CONNECT_WIFI,
  EVT_CMD_SET_FW_HOST,
  EVT_CMD_GET_FW_HOST,
  EVT_CMD_FW_UPDATE,
  EVT_FW_UPDATING,
  EVT_CMD_SET_SENSOR_CFG,
  EVT_CMD_GET_SENSOR_CFG,
  EVT_CMD_CALIBRATE,
  EVT_MAX
};

typedef struct {
  MyEventID id;
  uint8_t payload[PAYLOAD_SIZE];
  size_t length;
  unsigned long timestamp;
} MyEvent;

#endif // MYEVENT_H
