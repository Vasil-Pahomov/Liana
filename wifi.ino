#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASS"

#define WIFI_AP_SSID "Liana"
#define WIFI_CONNECTION_TIMEOUT 10000  //Timeout (in milliseconds) of waiting for WiFi connection
#define MDNS_NAME "liana"

#define DNS_PORT 53
IPAddress apIP(192, 168, 4, 1);

//not sure this is really necessary, since state should be found from the inside of WiFi class.
#define WIFISTATE_CONNECTING 0
#define WIFISTATE_CONNECTED 1
#define WIFISTATE_SOFTAP 2
byte wifiState;

DNSServer dnsServer;

void wifiSetUp()
{
  wifiState = WIFISTATE_CONNECTING;
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void wifiLoop()
{
  if (wifiState != WIFISTATE_CONNECTING) {
     webRun();
     wsRun();
     otaRun();
    dnsServer.processNextRequest();
  } else {
    if (millis() > WIFI_CONNECTION_TIMEOUT) {
      WiFi.disconnect();
      Serial.println("Fallback to Access Point");
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(WIFI_AP_SSID);
      //setting up captive portal
      dnsServer.start(DNS_PORT, "*", apIP);
      finishWiFiSetup();
      wifiState = WIFISTATE_SOFTAP;
    } else 
      if (WiFi.status() == WL_CONNECTED) {
      finishWiFiSetup();
      wifiState = WIFISTATE_CONNECTED;
    }
  }
}

void finishWiFiSetup()
{
  anim.run();
  Serial.print("IP address: ");Serial.println(WiFi.localIP());
  if (MDNS.begin(MDNS_NAME)) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  anim.run();
  webSetup();
  anim.run();
  wsSetup();
  anim.run();
  otaSetUp();
}
