#include "PWSConfig.h"
#include <Arduino.h>
String PWSConfig::init(Preferences *p) {
  preferences = p;
}

String PWSConfig::get_wifi_ssid() {
  return preferences->getString("wifi_ssid");
}