#include "PWSConfig.h"

String getWifiAsJson(Preferences* p) {
  String json = "{";
  json += "\"wifi_ssid\": \"" + p.getString("wifi_ssid") + "\",";
  json += "\"wifi_password\": \"" + p.getString("wifi_password") + "\"";
  json += "}";
}

String getMqttAsJson(Preferences* p) {
  String json = "{";
  json += "\"mqtt_host\": \"" + p->getString("mqtt_host") + "\",";
  json += "\"mqtt_port\": " + String(p->getUInt("mqtt_port")) + ",";
  json += "\"mqtt_username\": \"" + p->getString("mqtt_username") + "\",";
  json += "\"mqtt_password\": \"" + p->getString("mqtt_password") + "\",";
  json += "\"mqtt_id\": \"" + p->getString("mqtt_id") + "\",";

}

void PWSConfig::init(Preferences *p) {
  preferences = p;
};

String PWSConfig::get_wifi_ssid() {
  return preferences->getString("wifi_ssid");
}

void PWSConfig::set_wifi_ssid(String ssid) {
  preferences->putString("wifi_ssid", ssid);
}

String PWSConfig::get_wifi_password() {
  return preferences->getString("wifi_password");
}

void PWSConfig::set_wifi_password(String password) {
  preferences->putString("wifi_password", password);
}

String PWSConfig::get_mqtt_host() {
  return preferences->getString("mqtt_host");
}

void PWSConfig::set_mqtt_host(String host) {
  preferences->putString("mqtt_host", host);
}

uint32_t PWSConfig::get_mqtt_port() {
  return preferences->getUInt("mqtt_port");
}

void PWSConfig::set_mqtt_port(uint32_t port) {
  preferences->putUInt("mqtt_port", port);
}

String PWSConfig::get_mqtt_id() {
  return preferences->getString("mqtt_id");
}

void PWSConfig::set_mqtt_id(String id) {
  preferences->putString("mqtt_id", id);
}

String PWSConfig::get_mqtt_username() {
  return preferences->getString("mqtt_username");
}

void PWSConfig::set_mqtt_username(String username) {
  preferences->putString("mqtt_username", username);
}

String PWSConfig::get_mqtt_password() {
  return preferences->getString("mqtt_password");
}

void PWSConfig::set_mqtt_password(String password) {
  preferences->putString("mqtt_password", password);
}

String PWSConfig::get_mqtt_as_json() {
  String json = "{";
  json += "\"mqtt_host\": \"" + get_mqtt_host() + "\",";
  json += "\"mqtt_port\": " + String(get_mqtt_port()) + ",";
  json += "\"mqtt_username\": \"" + get_mqtt_username() + "\",";
  json += "\"mqtt_password\": \"" + get_mqtt_password() + "\",";
  json += "\"mqtt_id\": \"" + get_mqtt_id() + "\"";
  json += "}";
  return json;
}

String PWSConfig::get_wifi_as_json() {
  String json = "{";
  json += "\"wifi_ssid\": \"" + get_wifi_ssid() + "\",";
  json += "\"wifi_password\": \"" + get_wifi_password() + "\"";
  json += "}";
  return json;

}