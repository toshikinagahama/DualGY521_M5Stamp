#include "global.h"

bool IsConnected = false;         //スマホ・PCと接続しているかどうか
bool IsMeasStop = true;           //測定終了しているかどうか
bool IsFirmwareUpdating = false;  //ファームウェアのアップデート中かどうか
uint8_t state = STATE_WAIT_INIT;  //状態
uint8_t event = EVT_NOP;          //イベント
unsigned long time_start_btn = 0; //ボタン長押し検知用タイマー
char device_id[64] = "";
uint64_t timer_start_timeout = 0;
uint64_t timer_stop_timeout = 0;
std::string wifi_ssid = "";
std::string wifi_pw = "";