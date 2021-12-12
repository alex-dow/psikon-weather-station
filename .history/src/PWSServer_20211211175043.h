#ifndef PWSServer_h
#define PWSServer_h
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <ArduinoLog.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Update.h>
#include <PubSubClient.h>
#include "PWSConfig.h"

class PWSServer
{
public:
  void begin(AsyncWebServer* ws, Preferences* c);
  bool wifiConnect();
  bool mqttConnect();
  IPAddress getIPAddr();
  String getWifiSsid();
  String getWifiPassword();
  bool hasWifiConfig();
  bool hasMqttConfig();
  void portalMode();
  void sendStaticFile(String file, AsyncWebServerRequest *request);
  bool isConnected();
  bool isPortalEnabled();
  bool isWebappEnabled();

  bool hasWifiFailed();
  bool hasMqttFailed();

  void enableWebapp();
  void disableWebapp();

  void enablePortal();
  void disablePortal();

private:
  WiFiClient espClient;
  PubSubClient mqttClient;
  AsyncWebServer* webServer;
  IPAddress ipaddr;
  Preferences* config;
  bool portalEnabled;
  bool webappEnabled;
  bool wifiFailed;
  bool mqttFailed;

  void setupServer()
  {

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    webServer->on("/", HTTP_GET, [this] (AsyncWebServerRequest *request) {
      sendStaticFile("index.html", request);
    });

    webServer->on("/index.html", HTTP_GET, [this] (AsyncWebServerRequest *request) {
      sendStaticFile("index.html", request);
    });

    webServer->on("/main.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
      sendStaticFile("main.js", request);
    });

    webServer->on("/main.css", HTTP_GET, [this](AsyncWebServerRequest *request) {
      sendStaticFile("main.css", request);
    });

    webServer->on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
      Log.infoln("Getting status");
      String json = "{";
      json += "\"ipAddr\": \"" + getIPAddr().toString() + "\",";
      json += "\"wifiFailed\": " + String(hasWifiFailed()) + ",";
      json += "\"mqttFailed\": " + String(hasMqttFailed()) + ",";
      json += "\"hasWifiConfig\": " + String(hasWifiConfig()) + "";
      json += "}";

      request->send(200, "application/json", json);
    });

    webServer->on("/api/mqtt-settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
      Log.infoln("Getting MQTT settings");
      request->send(200, "application/json", getMqttAsJson(config));
    });

    webServer->on("/api/mqtt-settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
      Log.infoln("Saving MQTT settings");
      if (!request->hasParam("mqtt_host", true) || !request->hasParam("mqtt_port", true) || !request->hasParam("mqtt_id", true)) {
        request->send(400, "application/json", "{\"error\": true, \"message\": \"The MQTT hostname, port number, and ID are required fields.\"}");
      } else {
        config->putString("mqtt_host", request->getParam("mqtt_host", true)->value());
        config->putUInt("mqtt_port", request->getParam("mqtt_port", true)->value().toInt());
        config->putString("mqtt_id", request->getParam("mqtt_id", true)->value());

        if (request->hasParam("mqtt_username")) {
          config->putString("mqtt_username", request->getParam("mqtt_username", true)->value());
        }

        if (request->hasParam("mqtt_password")) {
          config->putString("mqtt_password", request->getParam("mqtt_password", true)->value());
        }

        request->send(200, "application/json", getMqttAsJson(config));
      }
    });

    webServer->on("/api/wifi-settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
      Log.infoln("Getting WiFi settings");
      String json = getWifiAsJson(config);
      request->send(200, "application/json", json);
    });

    webServer->on("/api/wifi-settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
      Log.infoln("Saving WiFi settings");
      if (!request->hasParam("wifi_ssid", true)) {
        request->send(400, "application/json", "{\"error\": true, \"message\": \"A WiFi SSID is required.\"}");
      } else {
        String wifi_ssid = request->getParam("wifi_ssid", true)->value();
        config->putString("wifi_ssid", wifi_ssid);

        if (request->hasParam("wifi_password", true))
        {
          String wifi_password = request->getParam("wifi_password", true)->value();
          config->putString("wifi_password", wifi_password);
        }

        request->send(200, "application/json", getWifiAsJson(config));
      }
    });

    webServer->on("/api/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
      Log.warningln("Restarting...");
      request->send(204, "text/plain", "");
      ESP.restart();
    });

    webServer->on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
      Log.warningln("Reseting...");
      config->clear();
      ESP.restart();
    });

    webServer->on("/api/available-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
      Log.infoln("Getting available networks");
      int status = 200;

      String json = "[";

      int n = WiFi.scanComplete();
      if (n == -2)
      {
        Log.infoln("  -- Starting scan");
        WiFi.scanNetworks(true);
        status = 202;
      }
      else if (n == -1)
      {
        Log.infoln("  -- Still scanning ... ");
        status = 202;
      }
      else if (n)
      {
        Log.infoln("  -- Scan complete");
        for (int i = 0; i < n; i++)
        {
          if (i)
            json += ",";
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

    webServer->on("^.*$", HTTP_GET, [this](AsyncWebServerRequest *request) {
      Log.infoln("Request url: %s", request->url().c_str());
      AsyncWebServerResponse *response = request->beginResponse(302);
      response->addHeader("Location", "/index.html");
      request->send(response);
    });
  }

};

#endif