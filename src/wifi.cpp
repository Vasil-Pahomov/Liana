#include <DNSServer.h>
#include <PersWiFiManager.h>
#include <ESP8266mDNS.h>

#include "web.h"
#include "websocket.h"

#define WIFI_AP_SSID "Liana"
#define WIFI_CONNECTION_TIMEOUT 10000  //Timeout (in milliseconds) of waiting for WiFi connection
#define MDNS_NAME "liana"

DNSServer dnsServer;
PersWiFiManager persWM(webServer, dnsServer);
bool _wifiConnected = false;

void wifiSetUp()
{
  webSetup();
  
  SPIFFS.begin();

  persWM.onConnect([]() {
    Serial.print("Connected, local IP:");
    Serial.println(WiFi.localIP());
    _wifiConnected = true;
  });
  persWM.onAp([](){
    Serial.println("AP MODE");
  });
  
  //sets network name for AP mode
  persWM.setApCredentials(WIFI_AP_SSID);

  //make connecting/disconnecting non-blocking
  persWM.setConnectNonBlock(true);

  //in non-blocking mode, program will continue past this point without waiting
  persWM.begin();

  if (!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error starting MDNS");
  }
  
  wsSetup();
}

void wifiLoop()
{
  persWM.handleWiFi();
  webServer.handleClient();
  wsRun();
  dnsServer.processNextRequest();
}

bool wifiIsConnected()
{
  return _wifiConnected;
}