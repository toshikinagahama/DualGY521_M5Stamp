#include "MyHandler.h"
#include "MyHandler_Advertise.h"
#include "MyHandler_Wait.h"
#include "MyHandler_Meas.h"
#include "MyHandler_FW.h"

EventHandler state_handler_table[STATE_MAX][EVT_MAX];

void init_state_handler_table() {
    for (int s = 0; s < STATE_MAX; s++) {
        for (int e = 0; e < EVT_MAX; e++) {
            state_handler_table[s][e] = NULL;
        }
    }

    // STATE_ADVERTISE
    state_handler_table[STATE_ADVERTISE][EVT_NOP]              = handler_adv_nop;
    state_handler_table[STATE_ADVERTISE][EVT_BLE_CONNECTED]    = handler_adv_ble_connected;
    state_handler_table[STATE_ADVERTISE][EVT_BLE_DISCONNECTED] = handler_adv_ble_disconnected;

    // STATE_WAIT
    state_handler_table[STATE_WAIT][EVT_NOP]                 = handler_wait_nop;
    state_handler_table[STATE_WAIT][EVT_BLE_DISCONNECTED]    = handler_wait_ble_disconnected;
    state_handler_table[STATE_WAIT][EVT_CMD_MEAS_START]      = handler_wait_cmd_meas_start;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_DEVICE_INFO] = handler_wait_cmd_get_device_info;
    state_handler_table[STATE_WAIT][EVT_CMD_SET_WIFI_SSID]   = handler_wait_cmd_set_wifi_ssid;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_WIFI_SSID]   = handler_wait_cmd_get_wifi_ssid;
    state_handler_table[STATE_WAIT][EVT_CMD_SET_WIFI_PW]     = handler_wait_cmd_set_wifi_pw;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_WIFI_PW]     = handler_wait_cmd_get_wifi_pw;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_WIFI_IP]     = handler_wait_cmd_get_wifi_ip;
    state_handler_table[STATE_WAIT][EVT_CMD_CONNECT_WIFI]    = handler_wait_cmd_connect_wifi;
    state_handler_table[STATE_WAIT][EVT_CMD_SET_FW_HOST]     = handler_wait_cmd_set_fw_host;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_FW_HOST]     = handler_wait_cmd_get_fw_host;
    state_handler_table[STATE_WAIT][EVT_CMD_FW_UPDATE]       = handler_wait_cmd_fw_update;
    state_handler_table[STATE_WAIT][EVT_CMD_SET_SENSOR_CFG]  = handler_wait_cmd_set_sensor_cfg;
    state_handler_table[STATE_WAIT][EVT_CMD_GET_SENSOR_CFG]  = handler_wait_cmd_get_sensor_cfg;

    // STATE_MEAS
    state_handler_table[STATE_MEAS][EVT_NOP]              = handler_meas_nop;
    state_handler_table[STATE_MEAS][EVT_CMD_MEAS_STOP]    = handler_meas_cmd_meas_stop;
    state_handler_table[STATE_MEAS][EVT_BLE_DISCONNECTED] = handler_meas_ble_disconnected;

    // STATE_FW_UPDATE
    state_handler_table[STATE_FW_UPDATE][EVT_NOP]              = handler_fw_nop;
    state_handler_table[STATE_FW_UPDATE][EVT_FW_UPDATING]      = handler_fw_updating;
    state_handler_table[STATE_FW_UPDATE][EVT_BLE_DISCONNECTED] = handler_fw_ble_disconnected;

    // STATE_FW_UPDATING
    state_handler_table[STATE_FW_UPDATING][EVT_NOP]              = handler_fw_nop;
    state_handler_table[STATE_FW_UPDATING][EVT_BLE_DISCONNECTED] = handler_fw_ble_disconnected;
}
