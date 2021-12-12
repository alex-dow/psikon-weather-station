#include <Arduino.h>

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

  if (server.isPortalEnabled()) {
    dnsServer.start(53, "*", server.getIPAddr());
  }
}

void loop() {
  if (server.isPortalEnabled()) {
    dnsServer.processNextRequest();
  } else {
    server.tick();
  }
}