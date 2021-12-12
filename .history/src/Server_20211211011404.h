#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <ArduinoLog.h>

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
    request->send(SPIFFS, "/index.html", "text/html", false);
  }
};

enum PWSServerState {
  PORTAL = 1,
  CONNECTING = 2,
  CONNECTED = 3,
  ERROR = 4
};

class PWSServer
{
public:
  void begin(AsyncWebServer *ws, Preferences *c)
  {
    webServer = ws;
    config = c;
    setupServer();
    if (!wifiConnect()) {
      portalMode();
    }
    ws->begin();

  }

  void init(AsyncWebServer *ws)
  {
    webServer = ws;
  }

  bool wifiConnect() {
    if (!hasWifiConfig()) {
      return false;
    }
    String wifi_ssid = getWifiSsid();
    String wifi_password = getWifiPassword();

    Log.infoln("Connecting to %s", wifi_ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Log.errorln("WiFi failed!");
      state = PWSServerState::ERROR;
      return false;
    } else {
      state = PWSServerState::CONNECTED;
      ipaddr = WiFi.localIP();
      return true;
    }
  }

  IPAddress* getIPAddr() {
    return &ipaddr;
  }

  String getWifiSsid() {
    return config->getString("wifi_ssid");
  }

  String getWifiPassword() {
    return config->getString("wifi_password");
  }

  bool hasWifiConfig() {
    return config->isKey("wifi_ssid") && config->getString("wifi_ssid").length() > 0;
  }

  void portalMode() {
    Log.infoln("Going into portal mode");
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP("Psikon Weather Station");
    IPAddress ip = WiFi.softAPIP();
    Log.infoln("AP IP: %s", ip.toString().c_str());
    state = PWSServerState::PORTAL;
    ipaddr = WiFi.softAPIP();
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

  bool isConnected() {
    return state == PWSServerState::CONNECTED;
  }

  bool isPortal() {
    return state == PWSServerState::PORTAL;
  }



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

    webServer->on("^.*$", HTTP_GET, [this](AsyncWebServerRequest *request) {
      Log.infoln("Request url: %s", request->url().c_str());
      AsyncWebServerResponse *response = request->beginResponse(302);
      response->addHeader("Location", "/index.html");
      request->send(response);
    });
  }

};