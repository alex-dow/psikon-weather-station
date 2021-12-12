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

#define SEALEVELPRESSURE_HPA (1013.25)



unsigned long delayTime = 5000;

Adafruit_BME280 bme(BME_CS); // hardware SPI
WiFiManager wifiManager;
PubSubClient client;

void get_mqtt_host() {
  File f = SPIFFS.open("/mqtt_host");
  if (!f) {
    return "";
  }

  uint8_t buffer[40];

  f.read(buffer, 40);

  return (char*) buffer;
}

void setup() {

  Serial.begin(115200);
  Serial.println("===============================================================================");
  Serial.println("= PSIKON WEATHER STATION                                                      =");
  Serial.println("===============================================================================");

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln("Logging initialized");

  Log.infoln("Starting SPIFFS");

  if (!SPIFFS.begin()) {
    Log.errorln("Unable to initialize SPIFFS");
  }



  wifiManager.resetSettings();

  wifiManager.setConnectTimeout(30);
  wifiManager.setConnectRetries(3);
  wifiManager.setTimeout(30);

  // MQTT Parameters
  WiFiManagerParameter mqtt_host("mqtt_host", "MQTT Host", "localhost", 40);
  WiFiManagerParameter mqtt_port("mqtt_port", "MQTT Port Number", "1883", 5);
  WiFiManagerParameter mqtt_username("mqtt_username", "MQTT Username", "", 40);
  WiFiManagerParameter mqtt_password("mqtt_password", "MQTT Password", "", 40);

  wifiManager.addParameter(&mqtt_host);
  wifiManager.addParameter(&mqtt_port);
  wifiManager.addParameter(&mqtt_username);
  wifiManager.addParameter(&mqtt_password);

  bool success = wifiManager.autoConnect("Psikon Weather Station #143");

  if (success) {
    Serial.println("Connected to wifi");
  } else {
    Serial.println("Not connected to wifi");
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


  uint16_t mqttport = strtol(mqtt_port.getValue(), NULL, 10);

  char *mqttid = (char*)malloc(20 * sizeof(char));
  sprintf(mqttid, "psikon_weather_%d", mqttid_number);

  Serial.print("MQTT ID: ");
  Serial.println(mqttid);

  Serial.print("MQTT Host: ");
  Serial.println(mqtt_host.getValue());

  Serial.print("MQTT Port: ");
  Serial.println(mqttport);

  Serial.print("MQTT Username: ");
  Serial.println(mqtt_username.getValue());

  Serial.print("MQTT Password: ");
  Serial.println(mqtt_password.getValue());

  client.setServer(mqtt_host.getValue(), mqttport);
  if (client.connect(mqttid, mqtt_username.getValue(), mqtt_password.getValue())) {
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

