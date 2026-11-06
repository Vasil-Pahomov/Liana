#ifndef liana_wifi_h
#define liana_wifi_h

#include <Arduino.h>

void wifiSetUp();

void wifiLoop();

bool wifiIsConnected();

// Unique per-unit suffix: the last 3 bytes of the station MAC as 6 uppercase hex digits, e.g. "A1B2C3".
String deviceSuffix();

// mDNS / network host name: MDNS_NAME + "-" + suffix in lowercase, e.g. "liana-a1b2c3" (liana-a1b2c3.local).
String deviceHostName();

#endif
