#include "common/MyFunc.h"
#include "common/MyGlobal.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void blinkLed(CRGB color, uint32_t onMs, uint32_t offMs)
{
  ledController.blink(color, onMs, offMs);
}

void connectWifiAP()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(sys.WIFI_SSID.c_str(), sys.WIFI_PW.c_str());
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(3000);
    ESP.restart();
  }
  sys.WIFI_IP = WiFi.localIP().toString().c_str();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void firmwareUpdate()
{
  ArduinoOTA.setHostname("myesp32");
  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
  enqueue(EVT_FW_UPDATING);
}
