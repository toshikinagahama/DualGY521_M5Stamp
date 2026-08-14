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
    if (data.length() < 2) return;

    // カテゴリ (data[0]) とコマンド種別 (data[1]) を 16bit 合成コードに変換 (例: 0x0200)
    uint16_t cmd = ((uint8_t)data[0] << 8) | (uint8_t)data[1];
    std::string payload = (data.length() > 2) ? data.substr(2) : "";

    switch (cmd)
    {
    case BLECmd::GET_DEVICE_INFO: enqueue(EVT_CMD_GET_DEVICE_INFO); break;
    case BLECmd::MEAS_START:      enqueue(EVT_CMD_MEAS_START); break;
    case BLECmd::MEAS_STOP:       enqueue(EVT_CMD_MEAS_STOP); break;

    // Wi-Fi コマンド
    case BLECmd::SET_WIFI_SSID:   (!payload.empty()) ? enqueue(EVT_CMD_SET_WIFI_SSID, payload.data(), payload.length()) : enqueue(EVT_CMD_GET_WIFI_SSID); break;
    case BLECmd::SET_WIFI_PW:     (!payload.empty()) ? enqueue(EVT_CMD_SET_WIFI_PW, payload.data(), payload.length()) : enqueue(EVT_CMD_GET_WIFI_PW); break;
    case BLECmd::GET_WIFI_IP:     enqueue(EVT_CMD_GET_WIFI_IP); break;
    case BLECmd::CONNECT_WIFI:    enqueue(EVT_CMD_CONNECT_WIFI); break;

    // Firmware / OTA コマンド
    case BLECmd::FW_UPDATE:      enqueue(EVT_CMD_FW_UPDATE); break;
    case BLECmd::SET_FW_HOST:    (!payload.empty()) ? enqueue(EVT_CMD_SET_FW_HOST, payload.data(), payload.length()) : enqueue(EVT_CMD_GET_FW_HOST); break;

    // GY-521 センサ設定 & キャリブレーション
    case BLECmd::SET_SENSOR_CFG: if (!payload.empty()) enqueue(EVT_CMD_SET_SENSOR_CFG, payload.data(), payload.length()); break;
    case BLECmd::GET_SENSOR_CFG: enqueue(EVT_CMD_GET_SENSOR_CFG); break;
    case BLECmd::CALIBRATE:      enqueue(EVT_CMD_CALIBRATE); break;

    default:
      Serial.printf("Unknown BLE Cmd: 0x%04X\n", cmd);
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
      Serial.println("auth failed -> disconnecting!");
      if (ble && ble->pServer)
      {
        ble->pServer->disconnect(0); // 認証失敗（PINコード間違い等）の時は即座に切断！
      }
    }
  }
};

static MySecurity secCallbacks;
static MyServerCallbacks serverCallbacks;
static MyCallbacks charCallbacks;
static BLE2902 descriptor2902;

void MyBLE::initialize()
{
  BLEDevice::init("DualGY521_M5Stamp");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
  BLEDevice::setSecurityCallbacks(&secCallbacks);

  BLESecurity security;
  security.setKeySize(16);
  security.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND); // MITM保護とPIN認証を必須化
  security.setCapability(ESP_IO_CAP_OUT);
  security.setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  uint32_t passkey = 123456;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);

  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  // キャラクタリスティックへのアクセスに MITM (PINコード認証付き暗号化) を必須に設定
  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENC_MITM | ESP_GATT_PERM_WRITE_ENC_MITM);

  pCharacteristic->setCallbacks(&charCallbacks);
  pCharacteristic->addDescriptor(&descriptor2902);

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