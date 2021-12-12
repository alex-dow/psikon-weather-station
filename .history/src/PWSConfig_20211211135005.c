#include "PWSConfig.h"

String PWSConfig::get_wifi_ssid() {
  return preferences->getString("wifi_ssid");
}