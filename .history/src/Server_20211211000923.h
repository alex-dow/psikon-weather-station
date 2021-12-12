#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>

class CaptivePortalHandler : public AsyncWebHandler
{
public:
  CaptivePortalHandler()
  {
    setupServer();
  }
  virtual ~CaptivePortalHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    // setupServer();

    request->send(SPIFFS, "/index.html", "text/html", false);
    /*
    String url = request->url();
    Log.infoln("Portal url: %s", url.c_str());
    if (url.equals("/bundle.css")) {
      request->send(SPIFFS, "/bundle.css", "text/css", false);
    } else if (url.equals("/bundle.js")) {
      request->send(SPIFFS, "/bundle.js", "text/javascript", false);
    }  else if (url.equals("/global.css")) {
      request->send(SPIFFS, "/global.css", "text/css", false);
    } else {
      request->send(SPIFFS, "/index.html", "text/html", false);
    }
    */
  }
};

class Server
{
public:
  void begin(AsyncWebServer *ws, Preferences *c)
  {
    webServer = ws;
    config = c;
  }

  void init(AsyncWebServer *ws)
  {
    webServer = ws;
  }

  void sendStaticFile(String file, AsyncWebServerRequest *request) {

    String contentType;

    if (file.endsWith(".css")) {
      contentType = "text/css";
    } else if (file.endsWith(".js")) {
      contentType = "text/javascript";
    } else if (file.endsWith(".html")) {
      contentType = "text/html";
    }

    request->send(SPIFFS, "/" + file, contentType, false);
  }


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
        wifi_ssid = request->getParam("wifi_ssid", true)->value();
        config->putString("wifi_ssid", wifi_ssid);
      }

      if (request->hasParam("wifi_password", true))
      {
        wifi_password = request->getParam("wifi_password", true)->value();
        config->putString("wifi_password", wifi_password);
      }

      json += "\"wifi_ssid\": \"" + wifi_ssid + "\"";
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
  }

private:
  AsyncWebServer* webServer;
  Preferences* config;
};