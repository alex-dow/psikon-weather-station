#include "PWSServer.h"
#include "PWSConfig.h"
#include <PubSubClient.h>

void PWSServer::begin(AsyncWebServer* ws, Preferences* c)
{
  bool webapp = false;
  espClient = WiFiClient();
  mqttClient = PubSubClient(espClient);
  bme = Adafruit_BME280(BME_CS);

  webServer = ws;
  config = c;
  setupServer();
  if (!wifiConnect()) {
    Log.warningln("Unable to connect to WiFi");
    webapp = true;
    enablePortal();
  }

  if (!bmeConnect()) {
    Log.warningln("Unable to connect to BME280 sensor");
    webapp = true;
  }

  if (!mqttConnect()) {
    Log.warningln("Unable to connect to MQTT");
    webapp = true;
  }

  if (webapp) {
    enableWebapp();
  }
}

bool PWSServer::bmeConnect() {
  if (bme.begin()) {
    bmeConnected = true;
  } else {
    bmeConnected = false;
  }

  return bmeConnected;
}

bool PWSServer::hasMqttConfig() {
  return config->isKey("mqtt_host") && config->isKey("mqtt_port");
}

bool PWSServer::mqttConnect() {
  if (!hasMqttConfig()) {
    return false;
  }
  Log.infoln("Setting MQTT Server");
  mqttClient.setServer(config->getString("mqtt_host").c_str(), config->getUInt("mqtt_port"));

  String mqtt_username = config->getString("mqtt_username");
  String mqtt_password = config->getString("mqtt_password");

  bool clientConnected;
  if (mqtt_username == NULL || mqtt_username.length() == 0) {
    Log.warningln("Connecting to MQTT with no username or password!");
    clientConnected = mqttClient.connect(config->getString("mqtt_id").c_str());
  } else {
    clientConnected  = mqttClient.connect(config->getString("mqtt_id").c_str(), mqtt_username.c_str(), mqtt_password.c_str());
  }

  if (clientConnected) {
    Serial.println("MQTT Connected");
  } else {
    Serial.println("Failed to connect to MQTT server");
  }

  return clientConnected;
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

bool PWSServer::isWifiConnected() {
  return WiFi.isConnected();
}

bool PWSServer::isMqttConnected() {
  return mqttClient.connected();
}

bool PWSServer::isBmeConnected() {
  return bme.
}
bool PWSServer::isPortalEnabled() {
  return portalEnabled;
}

bool PWSServer::isWebappEnabled() {
  return webappEnabled;
}

bool PWSServer::hasMqttFailed() {
  return mqttFailed;
}

bool PWSServer::hasWifiFailed() {
  return wifiFailed;
}

String PWSServer::getHumidityTopic() {
  if (topicHumidity.length() == 0) {
    topicHumidity = "/psikon/weather/" + config->getString("mqtt_id") + "/humidity";
  }
}