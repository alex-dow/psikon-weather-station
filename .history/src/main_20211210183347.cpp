#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
//#include "spiffs_db.h"
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);
Preferences config;

const char* ssid = "God Emperor of Verdun";
const char* password = "thisisnotapassword";

String wifi_ssid;
String wifi_password;




void setup() {
  Serial.begin(115200);
  Serial.println("===============================================================================");
  Serial.println("= PSIKON WEATHER STATION                                                      =");
  Serial.println("===============================================================================");

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln("Logging initialized");

  Log.info("Mounting SPIFFS ... ");
  if (!SPIFFS.begin()) {
    Log.errorln("Unable to initialize SPIFFS");
  } else {
    Log.infoln("SPIFFS mounted");
  }

  Log.info("Initializing config ... ");
  config.begin("psikon-weather", false);
  Log.infoln("done.");

  if (config.isKey("wifi_ssid")) {
    wifi_ssid = config.getString("wifi_ssid");
    wifi_password = config.getString("wifi_password");

    Log.info("Connecting to %s", wifi_ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Log.errorln("WiFi failed!");
      WiFi.mode(WIFI_AP);
      IPAddress ip = WiFi.softAPIP("Psikon Weather Station");
      Log.infoln("AP IP: %s", ip.toString());
    }

    Log.infoln("Local IP: %s", WiFi.localIP().toString());
  }

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  webServer.on("/wifi-settings", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String json = "{";


    json += "\"wifi_ssid\": \"" + wifi_ssid + "\"";
    json += "}";

    request->send(200, "application/json", json);
  });

  webServer.on("/wifi-settings", HTTP_POST, [] (AsyncWebServerRequest *request) {
    // Log.infoln("Saving wifi settings: %s / %s", request->getParam("wifi_ssid")->value(), request->getParam("wifi_password")->value());
    String json = "{";

    if (request->hasParam("wifi_ssid", true)) {
      wifi_ssid = request->getParam("wifi_ssid", true)->value();
      config.putString("wifi_ssid", wifi_ssid);
    }

    if (request->hasParam("wifi_password", true)) {
      wifi_password = request->getParam("wifi_password", true)->value();
      config.putString("wifi_password", wifi_password);
    }

    json += "\"wifi_ssid\": \"" + wifi_ssid + "\"";
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

    request->send(status, "application/json", json);
  });

  webServer.begin();
}

void loop() {
  delay(1000);
}