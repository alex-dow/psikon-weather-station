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

PWSServer::wifiConnect() {
  if (!hasWifiConfig()) {
    return false;
  }
  String wifi_ssid = getWifiSsid();
  String wifi_password = getWifiPassword();

  Log.infoln("Connecting to %s", wifi_ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Log.errorln("WiFi failed!");
    state = PWSServerState::ERROR;
    return false;
  } else {
    state = PWSServerState::CONNECTED;
    ipaddr = WiFi.localIP();
    return true;
  }
}

PWSServer::getIPAddr() {
  return ipaddr;
}

PWSServer::getWifiSsid() {
  return config->getString("wifi_ssid");
}