#ifndef GLOBAL_H

#ifdef GLOBAL_H
#define EXTERN extern
#else
#define EXTERN
#endif

#define GLOBAL_H

#define ADDRESS_MAJOR_VERSION 0x00
#define ADDRESS_MINOR_VERSION 0x01
#define ADDRESS_REVISION_VERSION 0x02

#include <Arduino.h>

/**
 * @brief 割り込みビット
 *
 */
struct INT_BIT_T
{
  uint8_t BLE_CONNECTED = 0;       // BLE端末と接続
  uint8_t BLE_DISCONNECTED = 0;    // BLE端末と切断
  uint8_t WIFI_CONNECTED = 0;      // WIFIと接続
  uint8_t WIFI_DISCONNECTED = 0;   // WIFIと切断
  uint8_t CMD_MEAS_START = 0;      //測定開始コマンド
  uint8_t CMD_MEAS_STOP = 0;       //測定終了コマンド
  uint8_t CMD_GET_DEVICE_INFO = 0; //機器情報取得コマンド
  uint8_t CMD_SET_WIFI_SSID = 0;   // WIFI SSID設定コマンド
  uint8_t CMD_GET_WIFI_SSID = 0;   // WIFI SSID取得コマンド
  uint8_t CMD_SET_WIFI_PW = 0;     // WIFI PW設定コマンド
  uint8_t CMD_GET_WIFI_PW = 0;     // WIFI PW取得コマンド
  uint8_t CMD_GET_WIFI_IP = 0;     // WIFI IP取得コマンド
  uint8_t CMD_CONNECT_WIFI = 0;    // WIFI 接続コマンド
  uint8_t CMD_FW_UPDATE = 0;       // FW更新コマンド
  uint8_t FW_UPDATE = 0;           // FW更新
  uint8_t FW_UPDATING = 0;         // FW更新中
  uint8_t CMD_SET_FW_HOST = 0;     // FWホスト設定コマンド
  uint8_t CMD_GET_FW_HOST = 0;     // FWホスト取得コマンド
};

/**
 * @brief 割り込み構造体
 *
 */
struct INT_T
{
  struct INT_BIT_T BIT; //ビットフィールド
};

/**
 * @brief システム構造体
 *
 */
struct SYS_T
{
  std::string WIFI_SSID = "";
  std::string WIFI_PW = "";
  std::string WIFI_IP = "";
  std::string FW_HOST = "";
  std::string DEVICE_SERIAL_NUM = "";
  std::string DEVICE_VERSION = "";
  std::string BLE_ARG = ""; // BLEコマンドの引数
};

//外部宣言
extern INT_T INT;
extern SYS_T SYS;

#endif // GLOBAL_H