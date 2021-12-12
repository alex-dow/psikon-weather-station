#ifndef PWSConfig_h
#define PWSConfig_h

#include <Preferences.h>

class PWSConfig {
  public:
    PWSConfig(Preferences *preferences);
    String get_wifi_ssid();
    String get_wifi_password();
    String get_mqtt_host();
    uint8_t get_mqtt_port();
    String get_mqtt_id();
    String get_mqtt_username();
    String get_mqtt_password();
    String get_wifi_as_json();
    String get mqtt_as_json();
    void set_wifi_ssid(String ssid);
    void set_wifi_password(String password);
    void set_mqtt_id(String id);
    void set_mqtt_host(String host);
    void set_mqtt_port(uint8_t port);
    void set_mqtt_username(String username);
    void set_mqtt_password(String password);
  private:
    Preferences *preferences;

};

#endif