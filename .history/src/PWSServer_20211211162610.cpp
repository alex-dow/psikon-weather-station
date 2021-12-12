#include "PWSServer.h"
#include "PWSConfig.h"

void PWSServer::begin(AsyncWebServer* ws, Preferences* c)
{
  webServer = ws;
  config = c;
  setupServer();
  if (!wifiConnect()) {
    enablePortal();

  }
  enableWebapp();
}

bool PWSServer::wifiConnect() {
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
    return false;
  } else {
    ipaddr = WiFi.localIP();
    Log.infoln("Wconnected to wifi network");
    portalEnabled = false;
    return true;
  }
}

IPAddress PWSServer::getIPAddr() {
  return ipaddr;
}

String PWSServer::getWifiSsid() {
  return config->getString("wifi_ssid");
}

String PWSServer::getWifiPassword() {
  return config->getString("wifi_password");
}

bool PWSServer::hasWifiConfig() {
  return config->isKey("wifi_ssid") && config->getString("wifi_ssid").length() > 0;
}

void PWSServer::enablePortal() {
  Log.infoln("Enabling wifi captive portal");
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Psikon Weather Station");
  IPAddress ip = WiFi.softAPIP();
  Log.infoln("AP IP: %s", ip.toString().c_str());
  ipaddr = WiFi.softAPIP();
  portalEnabled = true;
}

void PWSServer::enableWebapp() {
  Log.infoln("Enabling webapp");
  webServer->begin();
}

void PWSServer::disableWebapp() {
  Log.infoln("Disabling webapp");
  webServer->end();
}

void PWSServer::sendStaticFile(String file, AsyncWebServerRequest *request) {

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

bool PWSServer::isConnected() {
  return WiFi.isConnected();
}

bool PWSServer::isPortalEnabled() {
  return portalEnabled;
}

bool PWSServer::isWebappEnabled() {
  return webappEnabled;
}



