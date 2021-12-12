#include "PWSConfig.h"
#include <Preferences.h>

String getWifiAsJson(Preferences* p) {
  String json = "{";
  json += "\"wifi_ssid\": \"" + p->getString("wifi_ssid") + "\",";
  json += "\"wifi_password\": \"" + p->getString("wifi_password") + "\"";
  json += "}";
}

String getMqttAsJson(Preferences* p) {
  String json = "{";
  json += "\"mqtt_host\": \"" + p->getString("mqtt_host") + "\",";
  json += "\"mqtt_port\": " + String(p->getUInt("mqtt_port")) + ",";
  json += "\"mqtt_username\": \"" + p->getString("mqtt_username") + "\",";
  json += "\"mqtt_password\": \"" + p->getString("mqtt_password") + "\",";
  json += "\"mqtt_id\": \"" + p->getString("mqtt_id") + "\",";
  json += "}";
}