#include "states/StateWait.h"
#include <WiFi.h>
#include "common/MyGlobal.h"

void StateWait::onEnter()
{
  Serial.println("Entering STATE_WAIT");
}

MyState StateWait::handleEvent(const MyEvent *event)
{
  switch (event->id)
  {
  case EVT_BLE_DISCONNECTED:
    return STATE_ADVERTISE;

  case EVT_CMD_MEAS_START:
    return STATE_MEAS;

  case EVT_CMD_GET_DEVICE_INFO:
    if (WiFi.status() == WL_CONNECTED)
    {
      sys.WIFI_IP = WiFi.localIP().toString().c_str();
    }
    ble->notify(std::string("device_version,") + sys.DEVICE_VERSION);
    ble->notify(std::string("device_ip,") + (sys.WIFI_IP.empty() ? "未接続" : sys.WIFI_IP));
    return STATE_WAIT;

  case EVT_CMD_SET_WIFI_SSID:
    if (event->length > 0)
    {
      sys.WIFI_SSID = std::string((char *)event->payload, event->length);
      saveStringToEEPROM(ADDRESS_WIFI_SSID, sys.WIFI_SSID);
    }
    ble->notify("wifi_ssid," + sys.WIFI_SSID);
    return STATE_WAIT;

  case EVT_CMD_GET_WIFI_SSID:
    ble->notify("wifi_ssid," + sys.WIFI_SSID);
    return STATE_WAIT;

  case EVT_CMD_SET_WIFI_PW:
    if (event->length > 0)
    {
      sys.WIFI_PW = std::string((char *)event->payload, event->length);
      saveStringToEEPROM(ADDRESS_WIFI_PW, sys.WIFI_PW);
    }
    ble->notify("wifi_pw," + sys.WIFI_PW);
    return STATE_WAIT;

  case EVT_CMD_GET_WIFI_PW:
    ble->notify("wifi_pw," + sys.WIFI_PW);
    return STATE_WAIT;

  case EVT_CMD_GET_WIFI_IP:
    ble->notify("device_ip," + sys.WIFI_IP);
    return STATE_WAIT;

  case EVT_CMD_CONNECT_WIFI:
    connectWifiAP();
    return STATE_WAIT;

  case EVT_CMD_SET_FW_HOST:
    if (event->length > 0)
    {
      sys.FW_HOST = std::string((char *)event->payload, event->length);
      saveStringToEEPROM(ADDRESS_FW_HOST, sys.FW_HOST);
    }
    ble->notify("fw_host," + sys.FW_HOST);
    return STATE_WAIT;

  case EVT_CMD_GET_FW_HOST:
    ble->notify("fw_host," + sys.FW_HOST);
    return STATE_WAIT;

  case EVT_CMD_FW_UPDATE:
    firmwareUpdate();
    return STATE_FW_UPDATE;

  case EVT_CMD_SET_SENSOR_CFG:
    if (event->length >= 2)
    {
      uint8_t accRange = event->payload[0];
      uint8_t gyroRange = event->payload[1];

      six_sensor->applySettings(accRange, gyroRange);

      EEPROM.writeByte(ADDRESS_ACC_RANGE, six_sensor->accel_range);
      EEPROM.writeByte(ADDRESS_GYRO_RANGE, six_sensor->gyro_range);
      EEPROM.commit();
    }
    {
      char resBuf[128];
      snprintf(resBuf, sizeof(resBuf), "sensor_cfg,{\"accel_range\":%d,\"gyro_range\":%d}",
               six_sensor->accel_range, six_sensor->gyro_range);
      ble->notify(resBuf);
    }
    return STATE_WAIT;

  case EVT_CMD_GET_SENSOR_CFG:
    {
      char resBuf[128];
      snprintf(resBuf, sizeof(resBuf), "sensor_cfg,{\"accel_range\":%d,\"gyro_range\":%d}",
               six_sensor->accel_range, six_sensor->gyro_range);
      ble->notify(resBuf);
    }
    return STATE_WAIT;

  case EVT_CMD_CALIBRATE:
    {
      Serial.println("Starting sensor calibration...");
      six_sensor->calibrate();

      // EEPROM にオフセットデータ (48 Bytes) を保存
      EEPROM.writeBytes(ADDRESS_OFFSET_DATA, six_sensor->acc_offset, sizeof(six_sensor->acc_offset));
      EEPROM.writeBytes(ADDRESS_OFFSET_DATA + sizeof(six_sensor->acc_offset), six_sensor->gyro_offset, sizeof(six_sensor->gyro_offset));
      EEPROM.commit();

      ble->notify(std::string("calibrate,success"));
      Serial.println("Calibration completed and saved to EEPROM.");
    }
    return STATE_WAIT;

  default:
    return STATE_WAIT;
  }
}
