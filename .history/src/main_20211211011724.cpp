#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include "./Server.h"

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);
Preferences config;
DNSServer dnsServer;
PWSServer server;


String wifi_ssid;
String wifi_password;

/*
void setupServer() {

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  webServer.on("/index.html", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  webServer.on("/main.js", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.js", String(), false);
  });

  webServer.on("/main.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.css", String(), false);
  });

  webServer.on("/ping", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"ip_addr\": \"" + WiFi.softAPIP().toString() + "\"";
    json += "}";

    request->send(200, "application/json", json);
  });

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
    ESP.restart();
  });

  webServer.on("/restart", HTTP_POST, [] (AsyncWebServerRequest *request) {
    request->send(204, "text/plain", "");
    ESP.restart();
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
}

*/


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

  server.begin(&webServer, &config);

  if (server.isPortal()) {
    dnsServer.start(53, "*", server.getIPAddr());
  }

  /*
  if (config.isKey("wifi_ssid")) {
    wifi_ssid = config.getString("wifi_ssid");
    wifi_password = config.getString("wifi_password");

    Log.info("Connecting to %s", wifi_ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Log.errorln("WiFi failed!");
    } else {
      start_portal = false;
    }
  }

  if (start_portal) {
    Log.infoln("Starting portal");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Psikon Weather Station");
    IPAddress ip = WiFi.softAPIP();
    dnsServer.start(53, "*", ip);
    Log.infoln("AP IP: %s", ip.toString().c_str());
    webServer.addHandler(new CaptivePortalHandler()).setFilter(ON_AP_FILTER);
  } else {
    setupServer();
  }

  Log.infoln("Local IP: %s", WiFi.localIP().toString().c_str());



  webServer.begin();
  */
}

void loop() {
  if (server.isPortal()) {
    dnsServer.processNextRequest();
  }
}