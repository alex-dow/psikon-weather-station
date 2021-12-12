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

    char *json = "[";
    int n = WiFi.scanNetworks();

    char *buf;
    for (int i = 0; i < n; i++) {
      WiFi.SSID(i).toCharArray(buf, 32);
      asprintf(&json, "%s{\"ssid\":\"%s\"},", json, WiFi.SSID(i));
    }

    asprintf(&json, "%s]", json);

    request->send(200, "application/json", json);
  });

  webServer.begin();
}

void loop() {
  delay(10);
}