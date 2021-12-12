#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>
#include "spiffs_db.h"
#include <ESPAsyncWebServer.h>

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);

void setup() {
  Serial.begin(115200);
  Serial.println("===============================================================================");
  Serial.println("= PSIKON WEATHER STATION                                                      =");
  Serial.println("===============================================================================");

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln("Logging initialized");
}

void loop() {

}