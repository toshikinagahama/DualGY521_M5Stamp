#ifndef MYHANDLER_WAIT_H
#define MYHANDLER_WAIT_H

#include "MyHandler.h"

MyState handler_wait_nop(const MyEvent *event);
MyState handler_wait_ble_disconnected(const MyEvent *event);
MyState handler_wait_cmd_meas_start(const MyEvent *event);
MyState handler_wait_cmd_get_device_info(const MyEvent *event);
MyState handler_wait_cmd_set_wifi_ssid(const MyEvent *event);
MyState handler_wait_cmd_get_wifi_ssid(const MyEvent *event);
MyState handler_wait_cmd_set_wifi_pw(const MyEvent *event);
MyState handler_wait_cmd_get_wifi_pw(const MyEvent *event);
MyState handler_wait_cmd_get_wifi_ip(const MyEvent *event);
MyState handler_wait_cmd_connect_wifi(const MyEvent *event);
MyState handler_wait_cmd_set_fw_host(const MyEvent *event);
MyState handler_wait_cmd_get_fw_host(const MyEvent *event);
MyState handler_wait_cmd_fw_update(const MyEvent *event);
MyState handler_wait_cmd_set_sensor_cfg(const MyEvent *event);
MyState handler_wait_cmd_get_sensor_cfg(const MyEvent *event);

#endif // MYHANDLER_WAIT_H
