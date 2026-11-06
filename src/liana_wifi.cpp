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

//last 3 bytes of the factory station MAC, in the order they are printed (AA:BB:CC:A1:B2:C3 -> A1B2C3);
//read from the chip, so it is available before WiFi is started
String deviceSuffix()
{
#if defined(ESP8266)
  uint32_t id = ESP.getChipId() & 0xFFFFFF; // already MAC[3..5] as one big-endian number
#else
  uint64_t mac = ESP.getEfuseMac();       // MAC[0] is the lowest byte
  uint32_t id = ((uint32_t)((mac >> 24) & 0xFF) << 16) |
                ((uint32_t)((mac >> 32) & 0xFF) << 8) |
                 (uint32_t)((mac >> 40) & 0xFF);
#endif
  char buf[8];
  snprintf(buf, sizeof(buf), "%06X", (unsigned)id);
  return String(buf);
}

String deviceHostName()
{
  String name = String(MDNS_NAME) + "-" + deviceSuffix();
  name.toLowerCase();
  return name;
}

//SSID of the setup access point: WIFI_AP_SSID + "-" + suffix, so that several units don't clash
static String apSsid()
{
  return String(WIFI_AP_SSID) + "-" + deviceSuffix();
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

  //the same name for DHCP (router's client list) and mDNS; must be set before connecting
  String host = deviceHostName();
#if defined(ESP8266)
  WiFi.hostname(host);
#else
  WiFi.setHostname(host.c_str());
#endif

  //in non-blocking mode, program will continue past this point without waiting
  persWM.begin();

  Serial.print("mDNS name: "); Serial.print(host); Serial.println(".local");
  if (!MDNS.begin(host.c_str())) {
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
