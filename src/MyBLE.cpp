#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "MyBLE.h"
#include "global.h"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    IsConnected = true;
    Serial.println("Connected");
  };

  void onDisconnect(BLEServer *pServer)
  {
    IsConnected = false;
    pServer->startAdvertising();
    Serial.println("Disconnected");
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  // Read時
  void onRead(BLECharacteristic *pCharacteristic)
  {
    // pCharacteristic->setValue("Hello World!");
  }

  // Write時
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
        //機器情報取得
        pCharacteristic->setValue("device_version," + device_version);
        pCharacteristic->notify();
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
        //測定開始
        IsMeasStop = false;
        break;
      case 0x01:
        //測定終了
        IsMeasStop = true;
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
          wifi_ssid = data.substr(2);
          Serial.println(wifi_ssid.c_str());
        }
        pCharacteristic->setValue("wifi_ssid," + wifi_ssid);
        pCharacteristic->notify();
        break;
      case 0x01:
        // PW
        if (len_data > 2)
        {
          wifi_pw = data.substr(2);
          Serial.println(wifi_pw.c_str());
        }
        pCharacteristic->setValue("wifi_pw," + wifi_pw);
        pCharacteristic->notify();
        break;
      case 0x02:
        pCharacteristic->setValue("device_ip" + (std::string)(WiFi.localIP().toString().c_str()));
        pCharacteristic->notify();
      default:
        break;
      }
      break;
    case 0x03:
      // Firmware関係
      switch (data[1])
      {
      case 0x00:
        IsMeasStop = true;
        IsFirmwareUpdating = true;
        break;
      case 0x01:
        break;
      default:
        break;
      }
      break;
    default:
      Serial.println("Invalid cmd");
      break;
    }
    Serial.printf("%d", len_data);
    Serial.println("");
    // Serial.printf("%x", data[0]);
    // Serial.println("");
    // Serial.printf("%x", data[1]);
    // Serial.println("");
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
    // ペアリング時のPINの表示
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
      // ペアリング完了
      Serial.println("auth success");
    }
    else
    {
      // ペアリング失敗
      Serial.println("auth failed");
    }
  }
};

/**
 *
 * 初期化
 *
 */
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
  pBattery = pServer->createService(BatteryService);

  pBatteryCharacteristic = pBattery->createCharacteristic(BLEUUID((uint16_t)0x2A19), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pBatteryDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pBatteryCharacteristic->setValue("Percentage 0 - 100");
  pBatteryCharacteristic->addDescriptor(pBatteryDescriptor);
  pBatteryCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pAdvertising = pServer->getAdvertising();
  // pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->addServiceUUID(BatteryService);
}

/**
 *
 * アドバタイズスタート
 */
void MyBLE::advertiseStart()
{
  pService->start();
  pBattery->start();
  pAdvertising->start();
}

/**
 *
 * アドバタイズストップ
 */
void MyBLE::advertiseStop()
{
  pService->stop();
  pBattery->stop();
  pAdvertising->stop();
}

/**
 *
 * notify
 */
void MyBLE::notify(char *val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}

/**
 *
 * notify(string)
 */
void MyBLE::notify(std::string val)
{
  pCharacteristic->setValue(val);
  pCharacteristic->notify();
}