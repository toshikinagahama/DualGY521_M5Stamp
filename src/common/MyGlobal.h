#ifndef MYGLOBAL_H
#define MYGLOBAL_H

#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>
#include "MyState.h"
#include "MyEvent.h"
#include "MemoryMap.h"
#include "MyBLE.h"
#include "SixAxisSensor.h"

struct SYS_T {
  std::string WIFI_SSID = "";
  std::string WIFI_PW = "";
  std::string WIFI_IP = "";
  std::string FW_HOST = "";
  std::string DEVICE_SERIAL_NUM = "";
  std::string DEVICE_VERSION = "";
  std::string BLE_ARG = "";
};

extern MyState state;
extern SYS_T sys;
extern MyBLE *ble;
extern SixAxisSensor *six_sensor;
extern CRGB leds[1];

// Event Queue Interface
void init_event_queue();
void enqueue(MyEventID id, const void* payload = NULL, size_t length = 0);
MyEvent dequeue(TickType_t xTicksToWait = portMAX_DELAY);

// EEPROM Helper Functions
void saveStringToEEPROM(int address, const std::string &str, size_t maxLen = EEPROM_STR_MAX_LEN);
std::string loadStringFromEEPROM(int address, size_t maxLen = EEPROM_STR_MAX_LEN);
void init_eeprom_settings();

#endif // MYGLOBAL_H
