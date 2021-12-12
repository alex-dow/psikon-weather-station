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
#include <WiFi.h>
#include <AsyncTCP.h>

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

// #define RESET_ENABLED

#define SEALEVELPRESSURE_HPA (1013.25)



unsigned long delayTime = 5000;

Adafruit_BME280 bme(BME_CS); // hardware SPI
WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);




const char* get_spiff_data(const char* id, const char* defaultValue) {
  char fn[256];
  sprintf(fn, "/%s", id);
  File f = SPIFFS.open(fn);
  if (!f || f.size() == 0) {
    Log.warningln("SPIFFS - %s does not exist or is empty", fn);
    return defaultValue;
    //strcat(val, defaultValue);
  } else {
    size_t n = f.size();
    char* buf=(char *) malloc(n+1);

    f.read((uint8_t *) buf, n);
    buf[n] = '\0';
    // strcat(val, buf);
    Log.infoln("Fetched %s: %s\0", fn, buf);
    f.close();
    return buf;
  }

  // return buf;
  /*


  char buf[data.length() + 1];

  data.toCharArray(buf, data.length() + 1);



  return buf;

  // return data.toCharArray();
  */
}

void set_spiff_data(const char* id, const char* data) {
  char fn[256];
  sprintf(fn, "/%s", id);
  File f = SPIFFS.open(fn, "w");
  if (!f) {
    Log.warningln("Unable to open %s for writing", fn);
    return;
  }

  int bytesWritten = f.print(data);
  if (bytesWritten == 0) {
    Log.warningln("Wrote 0 bytes to %s", fn);
  } else {
    Log.infoln("Wrote %d bytes to %s: %s", bytesWritten, fn, data);
  }
  f.close();
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



  wifiManager.setConnectTimeout(30);
  wifiManager.setConnectRetries(3);

  // MQTT Parameters
  WiFiManagerParameter _mqtt_host("mqtt_host", "MQTT Host", "localhost", 40);
  WiFiManagerParameter _mqtt_port("mqtt_port", "MQTT Port Number", "1883", 5);
  WiFiManagerParameter _mqtt_username("mqtt_username", "MQTT Username", "", 40);
  WiFiManagerParameter _mqtt_password("mqtt_password", "MQTT Password", "", 40);

  wifiManager.addParameter(&_mqtt_host);
  wifiManager.addParameter(&_mqtt_port);
  wifiManager.addParameter(&_mqtt_username);
  wifiManager.addParameter(&_mqtt_password);

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  bool success = wifiManager.autoConnect("Psikon Weather Station #143");

  if (success) {
    Log.infoln("Connected to wifi");
  } else {
    Log.infoln("Not connected to wifi");
  }

  if (saveConfig) {
    Log.info("Saving updated mqtt configuration");
    set_spiff_data("mqtt_host", _mqtt_host.getValue());
    set_spiff_data("mqtt_port", _mqtt_port.getValue());
    set_spiff_data("mqtt_username", _mqtt_username.getValue());
    set_spiff_data("mqtt_password", _mqtt_password.getValue());
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

  const char* mqtt_host = get_spiff_data("mqtt_host", "localhost");
  const char* mqtt_port = get_spiff_data("mqtt_port", "1883");
  const char* mqtt_username = get_spiff_data("mqtt_username", "");
  const char* mqtt_password = get_spiff_data("mqtt_password", "");

  int port = atoi(mqtt_port);

  Log.infoln("MQTT ID: %s", mqttid);
  Log.infoln("MQTT URL: mqtt://%s:%d@%s:%s", mqtt_host, port, mqtt_username, mqtt_password);


  //long port = strtol(mqtt_port, NULL, 10);

  Log.infoln("Setting MQTT Server");
  client.setServer(mqtt_host, port);
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

  float pressure = bme.readPressure() / 100.F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
  float temp     = bme.readTemperature();

  client.publish("/psikon/")


  printValues();
  delay(delayTime);

  // put your main code here, to run repeatedly:
}

