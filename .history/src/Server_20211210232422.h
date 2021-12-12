#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

class CaptivePortalHandler : public AsyncWebHandler {
  public:
  CaptivePortalHandler() {
    setupServer();
  }
  virtual ~CaptivePortalHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    // setupServer();

    request->send(SPIFFS, "/index.html", "text/html", false);
    /*
    String url = request->url();
    Log.infoln("Portal url: %s", url.c_str());
    if (url.equals("/bundle.css")) {
      request->send(SPIFFS, "/bundle.css", "text/css", false);
    } else if (url.equals("/bundle.js")) {
      request->send(SPIFFS, "/bundle.js", "text/javascript", false);
    }  else if (url.equals("/global.css")) {
      request->send(SPIFFS, "/global.css", "text/css", false);
    } else {
      request->send(SPIFFS, "/index.html", "text/html", false);
    }
    */
  }
};

class Server {
  public:
    void begin(AsyncWebServer* ws) {
      webServer = ws;
    }
  private:
    AsyncWebServer* webServer;
};