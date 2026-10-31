#include <DNSServer.h>
#include <PersWiFiManager.h>
#if defined(ESP8266)
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#else
#error "Unsupported board class"
#endif

#include "liana_wifi.h"
#include "web.h"
#include "websocket.h"
#include "config.h"


DNSServer dnsServer;
PersWiFiManager persWM(webServer, dnsServer);
bool _wifiConnected = false;

//SSID of the setup access point: the configured one, or "Liana-" + chip id so that several units don't clash
static String apSsid()
{
#ifdef WIFI_AP_SSID
  return String(WIFI_AP_SSID);
#else
  #if defined(ESP8266)
  uint32_t id = ESP.getChipId();
  #else
  uint32_t id = (uint32_t)(ESP.getEfuseMac() >> 32);
  #endif
  char buf[16];
  snprintf(buf, sizeof(buf), "Liana-%04X", (unsigned)(id & 0xFFFF));
  return String(buf);
#endif
}

void wifiSetUp()
{
  Serial.println("Wifi is configuring");

  persWM.onConnect([]() {
    Serial.print("Connected, local IP:");
    Serial.println(WiFi.localIP());
    _wifiConnected = true;
  });
  persWM.onAp([](){
    Serial.println("AP MODE");
  });

  //sets network name for AP mode
  String ssid = apSsid();
  Serial.print("AP SSID: "); Serial.println(ssid);
  persWM.setApCredentials(ssid);

  //make connecting/disconnecting non-blocking
  persWM.setConnectNonBlock(true);

  WiFi.persistent(true);

  //in non-blocking mode, program will continue past this point without waiting
  persWM.begin();

  if (!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error starting MDNS");
  }

  webSetup();
  wsSetup();
  Serial.println("Wifi configured");
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
