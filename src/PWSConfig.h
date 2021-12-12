#ifndef PWSConfig_h
#define PWSConfig_h

#include <Preferences.h>

String getWifiAsJson(Preferences* preferences);
String getMqttAsJson(Preferences* preferences);

#endif