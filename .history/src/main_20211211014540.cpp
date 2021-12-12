#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <DNSServer.h>
#include "PWSServer.h"

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);
Preferences config;
DNSServer dnsServer;
PWSServer server;


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
    Log.infoln("done");
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