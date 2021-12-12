PWSServer::begin(AsyncWebServer* ws, Preferences* c)
{
  webServer = ws;
  config = c;
  setupServer();
  if (!wifiConnect()) {
    portalMode();
  }
  ws->begin();

}

