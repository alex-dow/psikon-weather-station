#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <SPI.h>
#include<PubSubClient.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

#define RESET_ENABLED

#define SEALEVELPRESSURE_HPA (1013.25)



unsigned long delayTime = 5000;

Adafruit_BME280 bme(BME_CS); // hardware SPI
WiFiManager wifiManager;
PubSubClient client;



void set_mqtt_host(const char* hostname) {
  File f = SPIFFS.open("/mqtt_host", "w");
  f.print(hostname);
  f.close();
}
void set_mqtt_port(const char* port) {
  File f = SPIFFS.open("/mqtt_port", "w");
  f.print(port);
  f.close();
}

void set_mqtt_username(const char* username) {
  File f = SPIFFS.open("/mqtt_username", "w");
  f.print(username);
  f.close();
}

void set_mqtt_password(const char* password) {
  File f = SPIFFS.open("/mqtt_password", "w");
  f.print(password);
  f.close();
}

const char* get_mqtt_host() {
  File f = SPIFFS.open("/mqtt_host");
  if (!f) {
    return "localhost";
  }

  uint8_t buffer[40];

  f.read(buffer, 40);

  f.close();

  return (char*) buffer;
}

const uint16_t get_mqtt_port() {
  File f = SPIFFS.open("/mqtt_port");
  if (!f) {
    return 1883;
  }

  uint8_t buffer[5];

  f.read(buffer, 5);

  f.close();

  return strtol((char*) buffer, NULL, 10);
}

const char* get_mqtt_username() {
  File f = SPIFFS.open("/mqtt_username");
  if (!f) {
    return "";
  }

  uint8_t buffer[40];
  f.read(buffer, 40);
  f.close();
  return (char*) buffer;
}

const char* get_mqtt_password() {
  File f = SPIFFS.open("/mqtt_password");
  if (!f) {
    return "";
  }

  uint8_t buffer[40];
  f.read(buffer, 40);
  f.close();

  return (char*) buffer;
}

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

  Log.info("Loading MQTT configuration ... ");

  const char* mqtt_host = get_mqtt_host();
  const uint16_t mqtt_port = get_mqtt_port();
  const char* mqtt_username = get_mqtt_username();
  const char* mqtt_password = get_mqtt_password();
  char mqtt_port_str[5];
  sprintf(mqtt_port_str, "%d", mqtt_port);

  Log.infoln("MQTT URL: mqtt://%s:%s@%s:%d", mqtt_host, mqtt_port, mqtt_username, mqtt_password);

  wifiManager.setConnectTimeout(30);
  wifiManager.setConnectRetries(3);

  // MQTT Parameters
  WiFiManagerParameter _mqtt_host("mqtt_host", "MQTT Host", mqtt_host, 40);
  WiFiManagerParameter _mqtt_port("mqtt_port", "MQTT Port Number", mqtt_port_str, 5);
  WiFiManagerParameter _mqtt_username("mqtt_username", "MQTT Username", mqtt_username, 40);
  WiFiManagerParameter _mqtt_password("mqtt_password", "MQTT Password", mqtt_password, 40);

  wifiManager.addParameter(&_mqtt_host);
  wifiManager.addParameter(&_mqtt_port);
  wifiManager.addParameter(&_mqtt_username);
  wifiManager.addParameter(&_mqtt_password);

  bool success = wifiManager.autoConnect("Psikon Weather Station #143");

  if (success) {
    Serial.println("Connected to wifi");
  } else {
    Serial.println("Not connected to wifi");
  }

  if (saveConfig) {
    Log.info("Saving updated mqtt configuration");
    set_mqtt_host(_mqtt_host.getValue());
    set_mqtt_port(_mqtt_port.getValue());
    set_mqtt_username(_mqtt_username.getValue());
    set_mqtt_password(_mqtt_password.getValue());
    Log.infoln("done");
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

  char *mqttid = (char*)malloc(20 * sizeof(char));
  sprintf(mqttid, "psikon_weather_%d", mqttid_number);

  Serial.print("MQTT ID: ");
  Serial.println(mqttid);

  Serial.print("MQTT Host: ");
  Serial.println(mqtt_host);

  Serial.print("MQTT Port: ");
  Serial.println(mqtt_port);

  Serial.print("MQTT Username: ");
  Serial.println(mqtt_username);

  Serial.print("MQTT Password: ");
  Serial.println(mqtt_password);

  client.setServer(mqtt_host, mqtt_port);
  if (client.connect(mqttid, mqtt_username, mqtt_password)) {
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
  printValues();
  delay(delayTime);

  // put your main code here, to run repeatedly:
}

