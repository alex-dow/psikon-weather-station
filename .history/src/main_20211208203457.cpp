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

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

#define SEALEVELPRESSURE_HPA (1013.25)



unsigned long delayTime = 5000;

Adafruit_BME280 bme(BME_CS); // hardware SPI
WiFiManager wifiManager;
PubSubClient client;


void setup() {

  Serial.begin(115200);
  Serial.println("===============================================================================");
  Serial.println("= PSIKON WEATHER STATION                                                      =");
  Serial.println("===============================================================================");

  wifiManager.setConnectTimeout(20);
  wifiManager.setConnectRetries(3);

  // MQTT Parameters
  WiFiManagerParameter mqtt_host("mqtt_host", "MQTT Host", "localhost", 40);
  WiFiManagerParameter mqtt_port("mqtt_port", "MQTT Port Number", 1883, 5);
  WiFiManagerParameter mqtt_username("mqtt_username", "MQTT Username", "", 40);
  WiFiManagerParameter mqtt_password("mqtt_password", "MQTT Password", "");

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

  client.setServer(&mqtt_host.getValue());


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
