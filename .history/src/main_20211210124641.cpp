#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include "spiffs_db.h"
#include <ESPAsyncWebServer.h>

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);

const char* ssid = "God Emperor of Verdun";
const char* password = "thisisnotapassword";

void setup() {
  Serial.begin(115200);
  Serial.println("===============================================================================");
  Serial.println("= PSIKON WEATHER STATION                                                      =");
  Serial.println("===============================================================================");

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln("Logging initialized");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

  webServer.on("/wifi", HTTP_GET, [] (AsyncWebServerRequest *request) {

    String json = "[";

    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
      request->send(202, "{\"networks\": []}");
    } else if (n == -1) {
      request->send(204, "");
    } else if (n) {
      char buf[32];
      for (int i = 0; i < n; i++) {
        if (i) json += ",";
        json += "{";
        json += "\"ssid\": \"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\": \"" + String(WiFi.RSSI(i)) + "\",";
        json += "\"bssid\": \"" + WiFi.BSSIDstr(i) + "\",";
        json += "\"channel\": " + String(WiFi.channel(i)) + ",";
        json += "\"secure\": " + String(WiFi.encryptionType(i)) + ";
        json += "}"
      }
      WiFi.scanDelete();
    }

    request->send(200, "application/json", json);
  });

  webServer.begin();
}

void loop() {
  delay(1000);
}