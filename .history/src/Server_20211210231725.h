#include <ESPAsyncWebServer.h>

class Server {
  public:
    void begin(AsyncWebServer* ws) {
      webServer = ws;
    }
  private:
    AsyncWebServer* webServer;
};