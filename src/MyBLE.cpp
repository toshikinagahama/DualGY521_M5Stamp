#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "MyBLE.h"
#include "common/MyGlobal.h"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    enqueue(EVT_BLE_CONNECTED);
    Serial.println("Connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    pServer->startAdvertising();
    enqueue(EVT_BLE_DISCONNECTED);
    Serial.println("Disconnected");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic)
  {
  }

  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string data = pCharacteristic->getValue();
    uint16_t len_data = (uint16_t)data.length();
    if (len_data < 2)
      return; //無効なコマンド
    switch (data[0])
    {
    case 0x00:
      //機器情報関連
      switch (data[1])
      {
      case 0x01:
        enqueue(EVT_CMD_GET_DEVICE_INFO);
        break;
      default:
        break;
      }
      break;
    case 0x01:
      //測定関係
      switch (data[1])
      {
      case 0x00:
        enqueue(EVT_CMD_MEAS_START);
        break;
      case 0x01:
        enqueue(EVT_CMD_MEAS_STOP);
        break;
      default:
        break;
      }
      break;
    case 0x02:
      // Wifi関係
      switch (data[1])
      {
      case 0x00:
        // SSID
        if (len_data > 2)
        {
          std::string arg = data.substr(2);
          enqueue(EVT_CMD_SET_WIFI_SSID, arg.data(), arg.length());
        }
        else
        {
          enqueue(EVT_CMD_GET_WIFI_SSID);
        }
        break;
      case 0x01:
        // PW
        if (len_data > 2)
        {
          std::string arg = data.substr(2);
          enqueue(EVT_CMD_SET_WIFI_PW, arg.data(), arg.length());
        }
        else
        {
          enqueue(EVT_CMD_GET_WIFI_PW);
        }
        break;
      case 0x02:
        enqueue(EVT_CMD_GET_WIFI_IP);
        break;
      case 0x03:
        enqueue(EVT_CMD_CONNECT_WIFI);
        break;
      default:
        break;
      }
      break;
    case 0x03:
      // Firmware関係
      switch (data[1])
      {
      case 0x00:
        enqueue(EVT_CMD_FW_UPDATE);
        break;
      case 0x01:
        if (len_data > 2)
        {
          std::string arg = data.substr(2);
          enqueue(EVT_CMD_SET_FW_HOST, arg.data(), arg.length());
        }
        else
        {
          enqueue(EVT_CMD_GET_FW_HOST);
        }
        break;
      }
      break;
    case 0x04:
      // GY-521 センサ設定関係
      switch (data[1])
      {
      case 0x00:
        if (len_data >= 3)
        {
          std::string arg = data.substr(2);
          enqueue(EVT_CMD_SET_SENSOR_CFG, arg.data(), arg.length());
        }
        break;
      case 0x01:
        enqueue(EVT_CMD_GET_SENSOR_CFG);
        break;
      }
      break;
    default:
      Serial.println("Invalid cmd");
      break;
    }
  }
};

// ペアリング処理用
class MySecurity : public BLESecurityCallbacks
{
  bool onConfirmPIN(uint32_t pin)
  {
    return false;
  }

  uint32_t onPassKeyRequest()
  {
    Serial.println("ONPassKeyRequest");
    return 123456;
  }

  void onPassKeyNotify(uint32_t pass_key)
  {
    Serial.println("onPassKeyNotify number");
    Serial.println(pass_key);
  }

  bool onSecurityRequest()
  {
    Serial.println("onSecurityRequest");
    return true;
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
  {
    Serial.println("onAuthenticationComplete");
    if (cmpl.success)
    {
      Serial.println("auth success");
    }
    else
    {
      Serial.println("auth failed");
    }
  }
};

void MyBLE::initialize()
{
  BLEDevice::init("DualGY521_M5Stamp");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
  BLEDevice::setSecurityCallbacks(new MySecurity());
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setKeySize(16);

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
  pSecurity->setCapability(ESP_IO_CAP_OUT);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  uint32_t passkey = 123456;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pAdvertising = pServer->getAdvertising();
}

void MyBLE::advertiseStart()
{
  pService->start();
  pAdvertising->start();
}

void MyBLE::advertiseStop()
{
  pService->stop();
  pAdvertising->stop();
}

void MyBLE::notify(char *val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}

void MyBLE::notify(std::string val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}

void MyBLE::notify(uint8_t *val, size_t len)
{
  pCharacteristic->setValue(val, len);
  pCharacteristic->notify();
}