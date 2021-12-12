#ifndef PWSConfig_h
#define PWSConfig_h

#include <Preferences.h>

String get_wifi_as_json(Preferences* preferences);
String get_mqtt_as_json(Preferences* preferences);

#endif