#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASS"

#define WIFI_AP_SSID "Liana"
#define WIFI_AP_PASS "ws2812"

void wifiSetUp()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);

 // ждем соединения:
  while (WiFi.status() != WL_CONNECTED && millis() < 30000) {
    delay(500);
    Serial.print(".");
  }
  
  if (millis() >= 30000) {
    WiFi.disconnect();
    Serial.println("Fallback to Access Point");
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  }

  Serial.print("IP address: ");Serial.println(WiFi.localIP());

   if (MDNS.begin("liana")) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
}
