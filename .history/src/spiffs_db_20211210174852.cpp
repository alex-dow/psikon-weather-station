#include <Arduino.h>
#include "spiffs_db.h"
#include <SPIFFS.h>
#include <ArduinoLog.h>


void psi_get_db_value(char* buf, const char* id, const char* defaultValue) {
  char fn[256];
  sprintf(fn, "/%s", id);
  File f = SPIFFS.open(fn);
  if (!f || f.size() == 0) {
    Log.warningln("SPIFFS - %s does not exist or is empty", fn);
    strcpy(buf, defaultValue);


  } else {
    size_t n = f.size();
    char* buffer =(char *) malloc(n+1);

    f.read((uint8_t *) buffer, n);
    buffer[n] = '\0';

    strcpy(buf, buffer);
    Log.infoln("Fetched %s: %s", fn, buf);
  }
  f.close();
}
void psi_set_db_value(const char* id, const char* value) {
  char fn[256];
  sprintf(fn, "/%s", id);
  File f = SPIFFS.open(fn, "w");
  if (!f) {
    Log.warningln("Unable to open %s for writing", fn);
    return;
  }

  int bytesWritten = f.print(value);
  if (bytesWritten == 0) {
    Log.warningln("Wrote 0 bytes to %s", fn);
  } else {
    Log.infoln("Wrote %d bytes to %s: %s", bytesWritten, fn, value);
  }
  f.close();
}

