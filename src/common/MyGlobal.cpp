#include "common/MyGlobal.h"

SYS_T sys;
static MyBLE ble_inst;
static SixAxisSensor sensor_inst;
MyBLE *ble = &ble_inst;
SixAxisSensor *six_sensor = &sensor_inst;
TaskHandle_t hSensorTask = NULL;

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
  EEPROM.writeByte(ADDRESS_MINOR_VERSION, 3);
  EEPROM.writeByte(ADDRESS_REVISION_VERSION, 0);
  EEPROM.commit();

  char char_version[64];
  snprintf(char_version, sizeof(char_version), "%d.%d.%d",
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
    six_sensor->applySettings(accRange, gyroRange);

    // オフセットデータ (48 Bytes) を EEPROM から読み込み
    EEPROM.readBytes(ADDRESS_OFFSET_DATA, six_sensor->acc_offset, sizeof(six_sensor->acc_offset));
    EEPROM.readBytes(ADDRESS_OFFSET_DATA + sizeof(six_sensor->acc_offset), six_sensor->gyro_offset, sizeof(six_sensor->gyro_offset));

    // NaN ガード (未校正時の 0xFF データ保護)
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (isnan(six_sensor->acc_offset[i][j]) || isnan(six_sensor->gyro_offset[i][j]) ||
            abs(six_sensor->acc_offset[i][j]) > 10.0f || abs(six_sensor->gyro_offset[i][j]) > 2000.0f)
        {
          six_sensor->acc_offset[i][j] = 0.0f;
          six_sensor->gyro_offset[i][j] = 0.0f;
        }
      }
    }
  }
  else
  {
    EEPROM.writeByte(ADDRESS_INIT_FLAG, EEPROM_INIT_MAGIC);
    EEPROM.writeByte(ADDRESS_ACC_RANGE, 0);       // ±2g
    EEPROM.writeByte(ADDRESS_GYRO_RANGE, 0);      // ±250dps
    memset(six_sensor->acc_offset, 0, sizeof(six_sensor->acc_offset));
    memset(six_sensor->gyro_offset, 0, sizeof(six_sensor->gyro_offset));
    EEPROM.writeBytes(ADDRESS_OFFSET_DATA, six_sensor->acc_offset, sizeof(six_sensor->acc_offset));
    EEPROM.writeBytes(ADDRESS_OFFSET_DATA + sizeof(six_sensor->acc_offset), six_sensor->gyro_offset, sizeof(six_sensor->gyro_offset));
    EEPROM.commit();

    six_sensor->applySettings(0, 0);
  }
}
