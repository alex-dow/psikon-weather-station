#include <Arduino.h>
#include <spiffs_db.h>
#include <SPIFFS.h>
#include <ArduinoLog.h>


void get_db_value(char* buf, const char* id, const char* defaultValue) {
  char fn[256];
  sprintf(fn, "/%s", id);
  File f = SPIFFS.open(fn);
  if (!f || f.size() == 0) {
    Log.warningln("SPIFFS - %s does not exist or is empty", fn);
    strcpy(buf, defaultValue);

  } else {
    size_t n = f.size();
    char* buffer =(char *) malloc(n+1);

    f.read((uint8_t *) bufer, n);
    buffer[n] = '\0';
    // strcat(val, buf);
    Log.infoln("Fetched %s: %s\0", fn, buffer);
    f.close();

    strcpy(buf, buffer);
  }
}
void set_db_value(const char* id, const char* value);

