#include "MyGlobal.h"

MyState state = STATE_ADVERTISE;
SYS_T sys;
MyBLE *ble = new MyBLE();
SixAxisSensor *six_sensor = new SixAxisSensor();
CRGB leds[1];

static QueueHandle_t xEventQueue = NULL;
static const uint8_t DEFAULT_PAYLOAD[1] = {0};

void init_event_queue()
{
  if (xEventQueue == NULL)
  {
    xEventQueue = xQueueCreate(20, sizeof(MyEvent));
  }
}

void enqueue(MyEventID id, const void* payload, size_t length)
{
  init_event_queue();

  MyEvent event;
  event.id = id;
  if (payload == NULL || length == 0)
  {
    memcpy(event.payload, DEFAULT_PAYLOAD, 1);
    event.length = 0;
  }
  else
  {
    size_t copy_len = (length > PAYLOAD_SIZE) ? PAYLOAD_SIZE : length;
    memcpy(event.payload, payload, copy_len);
    event.length = copy_len;
  }
  event.timestamp = millis();

  // ESP32 (FreeRTOS) の割り込み文脈か判定
  if (xPortInIsrContext())
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR(xEventQueue, &event, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
      portYIELD_FROM_ISR();
    }
  }
  else
  {
    xQueueSendToBack(xEventQueue, &event, portMAX_DELAY);
  }
}

MyEvent dequeue(TickType_t xTicksToWait)
{
  init_event_queue();
  MyEvent event;

  if (xQueueReceive(xEventQueue, &event, xTicksToWait) == pdPASS)
  {
    return event;
  }

  MyEvent empty_event;
  empty_event.id = EVT_NOP;
  empty_event.length = 0;
  empty_event.timestamp = millis();
  return empty_event;
}

void saveStringToEEPROM(int address, const std::string &str, size_t maxLen)
{
  for (size_t i = 0; i < maxLen; i++)
  {
    if (i < str.length())
    {
      EEPROM.writeByte(address + i, (uint8_t)str[i]);
    }
    else
    {
      EEPROM.writeByte(address + i, 0);
    }
  }
  EEPROM.commit();
}

std::string loadStringFromEEPROM(int address, size_t maxLen)
{
  char buf[EEPROM_STR_MAX_LEN + 1];
  for (size_t i = 0; i < maxLen; i++)
  {
    uint8_t b = EEPROM.readByte(address + i);
    if (b == 0 || b == 0xFF)
    {
      buf[i] = '\0';
      break;
    }
    buf[i] = (char)b;
  }
  buf[maxLen] = '\0';
  return std::string(buf);
}

void init_eeprom_settings()
{
  if (!EEPROM.begin(EEPROM_TOTAL_SIZE))
  {
    Serial.println("Failed to initialise EEPROM. Restarting...");
    delay(1000);
    ESP.restart();
  }

  EEPROM.writeByte(ADDRESS_MAJOR_VERSION, 0);
  EEPROM.writeByte(ADDRESS_MINOR_VERSION, 2);
  EEPROM.writeByte(ADDRESS_REVISION_VERSION, 1);
  EEPROM.commit();

  char char_version[64];
  sprintf(char_version, "%d.%d.%d",
          EEPROM.readByte(ADDRESS_MAJOR_VERSION),
          EEPROM.readByte(ADDRESS_MINOR_VERSION),
          EEPROM.readByte(ADDRESS_REVISION_VERSION));
  sys.DEVICE_VERSION = char_version;

  if (EEPROM.readByte(ADDRESS_INIT_FLAG) == EEPROM_INIT_MAGIC)
  {
    sys.WIFI_SSID = loadStringFromEEPROM(ADDRESS_WIFI_SSID);
    sys.WIFI_PW   = loadStringFromEEPROM(ADDRESS_WIFI_PW);
    sys.FW_HOST   = loadStringFromEEPROM(ADDRESS_FW_HOST);

    uint8_t accRange = EEPROM.readByte(ADDRESS_ACC_RANGE);
    uint8_t gyroRange = EEPROM.readByte(ADDRESS_GYRO_RANGE);
    uint8_t motionThresh = EEPROM.readByte(ADDRESS_MOTION_THRESH);
    six_sensor->applySettings(accRange, gyroRange, motionThresh);
  }
  else
  {
    EEPROM.writeByte(ADDRESS_INIT_FLAG, EEPROM_INIT_MAGIC);
    EEPROM.writeByte(ADDRESS_ACC_RANGE, 0);       // ±2g
    EEPROM.writeByte(ADDRESS_GYRO_RANGE, 0);      // ±250dps
    EEPROM.writeByte(ADDRESS_MOTION_THRESH, 1);   // 動作検出スレッショルド 1
    EEPROM.commit();

    six_sensor->applySettings(0, 0, 1);
  }
}
