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

char* wifi_ssid;
char* wifi_password;




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

    psi_get_db_value(wifi_ssid, "wifi_ssid", "");
    psi_get_db_value(wifi_password, "wifi_password", "");

  webServer.on("/wifi-settings", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String json = "{";

    json += "\"wifi_ssid\": \"" + String(wifi_ssid) + "\"";
    json += "}";

    request->send(200, "application/json", json);
  });

  webServer.on("/wifi-settings", HTTP_POST, [] (AsyncWebServerRequest *request) {
    String json = "{";

    if (request->hasParam("wifi_ssid")) {
      request->getParam("wifi_ssid", true)->value().toCharArray(wifi_ssid, 32, 0);
      psi_set_db_value("wifi_ssid", wifi_ssid);
    }

    if (request->hasParam("wifi_password")) {
      request->getParam("wifi_password", true)->value().toCharArray(wifi_password, 256, 0);
      psi_set_db_value("wifi_password", wifi_password);
    }

    json += "\"wifi_ssid\": \"" + String(wifi_ssid) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  webServer.on("/available-networks", HTTP_GET, [] (AsyncWebServerRequest *request) {

    int status = 200;

    String json = "[";

    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
      status = 202;
    } else if (n == -1) {
      status = 202;
    } else if (n) {
      for (int i = 0; i < n; i++) {
        if (i) json += ",";
        json += "{";
        json += "\"ssid\": \"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\": " + String(WiFi.RSSI(i)) + ",";
        json += "\"bssid\": \"" + WiFi.BSSIDstr(i) + "\",";
        json += "\"channel\": " + String(WiFi.channel(i)) + ",";
        json += "\"secure\": " + String(WiFi.encryptionType(i));
        json += "}";
      }
      WiFi.scanDelete();
    }

    json += "]";

    AsyncWebServerResponse *response = request->beginResponse(status, "application/json", json);
    response->addHeader("Access-Control-Allow-Origin", "*");

    request->send(response);
  });

  webServer.begin();
}

void loop() {
  delay(1000);
}