#include "MyHandler_Wait.h"

MyState handler_wait_nop(const MyEvent *event) {
  return STATE_WAIT;
}

MyState handler_wait_ble_disconnected(const MyEvent *event) {
  Serial.println("State -> STATE_ADVERTISE");
  return STATE_ADVERTISE;
}

MyState handler_wait_cmd_meas_start(const MyEvent *event) {
  Serial.println("State -> STATE_MEAS");
  return STATE_MEAS;
}

MyState handler_wait_cmd_get_device_info(const MyEvent *event) {
  ble->notify("device_version," + sys.DEVICE_VERSION);
  return STATE_WAIT;
}

MyState handler_wait_cmd_set_wifi_ssid(const MyEvent *event) {
  if (event->length > 0) {
    sys.WIFI_SSID = std::string((char*)event->payload, event->length);
    saveStringToEEPROM(ADDRESS_WIFI_SSID, sys.WIFI_SSID);
  }
  ble->notify("wifi_ssid," + sys.WIFI_SSID);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_wifi_ssid(const MyEvent *event) {
  ble->notify("wifi_ssid," + sys.WIFI_SSID);
  return STATE_WAIT;
}

MyState handler_wait_cmd_set_wifi_pw(const MyEvent *event) {
  if (event->length > 0) {
    sys.WIFI_PW = std::string((char*)event->payload, event->length);
    saveStringToEEPROM(ADDRESS_WIFI_PW, sys.WIFI_PW);
  }
  ble->notify("wifi_pw," + sys.WIFI_PW);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_wifi_pw(const MyEvent *event) {
  ble->notify("wifi_pw," + sys.WIFI_PW);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_wifi_ip(const MyEvent *event) {
  ble->notify("device_ip," + sys.WIFI_IP);
  return STATE_WAIT;
}

MyState handler_wait_cmd_connect_wifi(const MyEvent *event) {
  connectWifiAP();
  return STATE_WAIT;
}

MyState handler_wait_cmd_set_fw_host(const MyEvent *event) {
  if (event->length > 0) {
    sys.FW_HOST = std::string((char*)event->payload, event->length);
    saveStringToEEPROM(ADDRESS_FW_HOST, sys.FW_HOST);
  }
  ble->notify("fw_host," + sys.FW_HOST);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_fw_host(const MyEvent *event) {
  ble->notify("fw_host," + sys.FW_HOST);
  return STATE_WAIT;
}

MyState handler_wait_cmd_fw_update(const MyEvent *event) {
  firmwareUpdate();
  return STATE_FW_UPDATE;
}

MyState handler_wait_cmd_set_sensor_cfg(const MyEvent *event) {
  if (event->length >= 3) {
    uint8_t accRange = event->payload[0];
    uint8_t gyroRange = event->payload[1];
    uint8_t motionThresh = event->payload[2];

    six_sensor->applySettings(accRange, gyroRange, motionThresh);

    EEPROM.writeByte(ADDRESS_ACC_RANGE, six_sensor->accel_range);
    EEPROM.writeByte(ADDRESS_GYRO_RANGE, six_sensor->gyro_range);
    EEPROM.writeByte(ADDRESS_MOTION_THRESH, six_sensor->motion_threshold);
    EEPROM.commit();
  }

  char resBuf[128];
  sprintf(resBuf, "sensor_cfg,{\"accel_range\":%d,\"gyro_range\":%d,\"motion_threshold\":%d}",
          six_sensor->accel_range, six_sensor->gyro_range, six_sensor->motion_threshold);
  ble->notify(resBuf);
  return STATE_WAIT;
}

MyState handler_wait_cmd_get_sensor_cfg(const MyEvent *event) {
  char resBuf[128];
  sprintf(resBuf, "sensor_cfg,{\"accel_range\":%d,\"gyro_range\":%d,\"motion_threshold\":%d}",
          six_sensor->accel_range, six_sensor->gyro_range, six_sensor->motion_threshold);
  ble->notify(resBuf);
  return STATE_WAIT;
}
