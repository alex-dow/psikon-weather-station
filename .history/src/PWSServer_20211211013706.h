#ifndef PWSServer_h
#define PWSServer_h
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <ArduinoLog.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Update.h>

enum PWSServerState {
  PORTAL = 1,
  CONNECTING = 2,
  CONNECTED = 3,
  ERROR = 4
};

class PWSServer
{
public:
  void begin(AsyncWebServer* ws, Preferences* c);
  bool wifiConnect();
  IPAddress getIPAddr();
  String getWifiSsid();
  String getWifiPassword();
  bool hasWifiConfig();
  void portalMode();
  void sendStaticFile(String file, AsyncWebServerRequest *request);
  bool isConnected();
  bool isPortal();

private:
  AsyncWebServer* webServer;
  Preferences* config;
  PWSServerState state;
  IPAddress ipaddr;
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

    webServer->on("/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
      String json = "{";
      json += "\"ip_addr\": \"" + WiFi.softAPIP().toString() + "\"";
      json += "}";

      request->send(200, "application/json", json);
    });

    webServer->on("/wifi-settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
      String json = "{";

      json += "\"wifi_ssid\": \"" + config->getString("wifi_ssid") + "\"";
      json += "}";

      request->send(200, "application/json", json);
    });

    webServer->on("/wifi-settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
      // Log.infoln("Saving wifi settings: %s / %s", request->getParam("wifi_ssid")->value(), request->getParam("wifi_password")->value());
      String json = "{";

      if (request->hasParam("wifi_ssid", true))
      {
        String wifi_ssid = request->getParam("wifi_ssid", true)->value();
        config->putString("wifi_ssid", wifi_ssid);
        json += "\"wifi_ssid\": \"" + wifi_ssid + "\"";
      }

      if (request->hasParam("wifi_password", true))
      {
        String wifi_password = request->getParam("wifi_password", true)->value();
        config->putString("wifi_password", wifi_password);
      }


      json += "}";
      request->send(200, "application/json", json);
      ESP.restart();
    });

    webServer->on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(204, "text/plain", "");
      ESP.restart();
    });

    webServer->on("/available-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
      int status = 200;

      String json = "[";

      int n = WiFi.scanComplete();
      if (n == -2)
      {
        WiFi.scanNetworks(true);
        status = 202;
      }
      else if (n == -1)
      {
        status = 202;
      }
      else if (n)
      {
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