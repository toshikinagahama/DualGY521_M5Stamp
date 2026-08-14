#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#define ADDRESS_MAJOR_VERSION    0x00
#define ADDRESS_MINOR_VERSION    0x01
#define ADDRESS_REVISION_VERSION 0x02

#define ADDRESS_INIT_FLAG        0x10
#define ADDRESS_WIFI_SSID        0x20
#define ADDRESS_WIFI_PW          0x60
#define ADDRESS_FW_HOST          0xA0

#define ADDRESS_ACC_RANGE        0xE0
#define ADDRESS_GYRO_RANGE       0xE1
#define ADDRESS_MOTION_THRESH    0xE2

#define EEPROM_INIT_MAGIC        0xA5
#define EEPROM_STR_MAX_LEN       64
#define EEPROM_TOTAL_SIZE        1000

#endif // MEMORY_MAP_H
