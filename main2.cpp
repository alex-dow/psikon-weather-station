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

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

//#define RESET_ENABLED

#define SEALEVELPRESSURE_HPA (1013.25)



unsigned long delayTime = 5000;
char temp_topic[256];
char humidity_topic[256];
char pressure_topic[256];
char altitude_topic[256];

Adafruit_BME280 bme(BME_CS); // hardware SPI
WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);


bool saveConfig = false;

void saveConfigCallback() {
  saveConfig = true;
}

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

  #ifdef RESET_ENABLED
  Log.warning("Formatting SPIFFS ... ");
  if (SPIFFS.format()) {
    Log.warningln("done");
   } else {
     Log.warningln("Failed to format SPIFFS");
   }
  Log.warningln("Clearning WIFI settings ... ");
  wifiManager.resetSettings();
  Log.warningln("done");
  #endif


  wifiManager.setConnectTimeout(30);
  wifiManager.setConnectRetries(3);

  char defaultId[128];
  sprintf(defaultId, "psikon-weather-%d", rand() % 99999);

  // MQTT Parameters
  WiFiManagerParameter _mqtt_host("mqtt_host", "MQTT Host", "localhost", 40);
  WiFiManagerParameter _mqtt_port("mqtt_port", "MQTT Port Number", "1883", 5);
  WiFiManagerParameter _mqtt_username("mqtt_username", "MQTT Username", "", 40);
  WiFiManagerParameter _mqtt_password("mqtt_password", "MQTT Password", "", 40);
  WiFiManagerParameter _me("me", "Name", defaultId, 128);

  wifiManager.addParameter(&_mqtt_host);
  wifiManager.addParameter(&_mqtt_port);
  wifiManager.addParameter(&_mqtt_username);
  wifiManager.addParameter(&_mqtt_password);
  wifiManager.addParameter(&_me);

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  bool success = wifiManager.autoConnect("Psikon Weather Station #143");

  if (success) {
    Log.infoln("Connected to wifi");
  } else {
    Log.infoln("Not connected to wifi");
  }

  if (saveConfig) {
    Log.info("Saving updated mqtt configuration");
    set_db_value("mqtt_host", _mqtt_host.getValue());
    set_db_value("mqtt_port", _mqtt_port.getValue());
    set_db_value("mqtt_username", _mqtt_username.getValue());
    set_db_value("mqtt_password", _mqtt_password.getValue());
    set_db_value("me", _me.getValue());
    Log.infoln("done");

    saveConfig = false;
    // ESP.restart();
  }

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  Serial.println("Connect to BME280");
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("BME280 started");
  Serial.println("Connect to MQTT");

  int mqttid_number = rand() % 99999;
  Serial.print("MQTT ID #:");
  Serial.println(mqttid_number);

  char mqttid[25];
  sprintf(mqttid, "psikon_weather_%d", mqttid_number);

  Log.info("Loading MQTT configuration ... ");

/*
  char* mqtt_host;
  char* mqtt_port;
  char* mqtt_username;
  char* mqtt_password;

  get_spiff_data(mqtt_host, "mqtt_host", "localhost");
  get_spiff_data(mqtt_port, "mqtt_port", "1883");
  get_spiff_data(mqtt_username, "mqtt_username", "");
  get_spiff_data(mqtt_password, "mqtt_password", "");
*/

  char mqtt_host[40];
  get_db_value(mqtt_host, "mqtt_host", "localhost");

  char mqtt_port[5];
  get_db_value(mqtt_port, "mqtt_port", "1883");

  char mqtt_username[40];
  get_db_value(mqtt_username, "mqtt_username", "");

  char mqtt_password[40];
  get_db_value(mqtt_password, "mqtt_password", "");

  char me[256];
  get_db_value(me, "me", "psikon-weather");

  sprintf(temp_topic, "/psikon/weather/%s/temperature", me);
  sprintf(humidity_topic, "/psikon/weather/%s/humidity", me);
  sprintf(pressure_topic, "/psikon/weather/%s/pressure", me);
  sprintf(altitude_topic, "/psikon/weather/%s/altitude", me);

  int port = atoi(mqtt_port);

  Log.infoln("MQTT ID: %s", me);
  Log.infoln("MQTT URL: mqtt://%s:%d@%s:%s", mqtt_host, port, mqtt_username, mqtt_password);


  //long port = strtol(mqtt_port, NULL, 10);

  Log.infoln("Setting MQTT Server");
  client.setServer(mqtt_host, port);
  bool clientConnected;
  if (strlen(mqtt_username) == 0) {
    Log.warningln("Connecting to MQTT with no username or password!");
    clientConnected = client.connect(me);
  } else {
    Log.infoln("Username length: %d", strlen(mqtt_username));
    clientConnected  = client.connect(me, mqtt_username, mqtt_password);
  }

  if (clientConnected) {
    Serial.println("MQTT Connected");
  } else {
    Serial.println("Failed to connect to MQTT server");
  }

  Serial.println();
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void loop() {

  float pressure = bme.readPressure() / 100.F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
  float temp     = bme.readTemperature();


  char buffer[64];
  sprintf(buffer, "%f", temp);
  client.publish(temp_topic, buffer);

  sprintf(buffer, "%f", humidity);
  client.publish(humidity_topic, buffer);

  sprintf(buffer, "%f", pressure);
  client.publish(pressure_topic, buffer);

  sprintf(buffer, "%f", altitude);
  client.publish(altitude_topic, buffer);


  printValues();
  delay(delayTime);

  // put your main code here, to run repeatedly:
}

