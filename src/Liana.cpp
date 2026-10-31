#include <WebSocketsServer.h>
#include <PersWiFiManager.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#else
#error "Unsupported board class"
#endif
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ezTime.h>

#include "fs_compat.h"
#include "config.h"
#include "settings.h"
#include "palette.h"
#include "anim.h"
#include "player.h"
#include "web.h"
#include "websocket.h"
#include "liana_wifi.h"
#include "mqtt.h"
#include "ota.h"
#include "hw_button.h"
#include "hw_led.h"

Timezone tz;

void setup() {
  Serial.begin(115200);

  Serial.println("Entering setup");
  if (LIANA_FS.begin()) {
    Serial.println("File system started");
  } else {
    Serial.println("File system FAILED");
  }
  settings.load();

  wifiSetUp();
  mqttSetup();

  // a POSIX rule is applied locally; setLocation() would need the network, which is not up yet
  tz.setPosix(F(TIMEZONE_POSIX));

  randomSeed(analogRead(0)*analogRead(1));
  anim.begin();
  player.begin();

  otaSetUp();
  ledSetUp();
  buttonsSetUp();
  Serial.println("Setup done");
}

void loop() {
  yield();

  anim.run();
  player.tick();

  wifiLoop();
  mqttRun();
  otaLoop();
  events();
  buttonsLoop();
}

unsigned int rng() {
    static unsigned int y = 0;
    y += micros(); // seeded with changing number
    y ^= y << 2; y ^= y >> 7; y ^= y << 7;
    return (y);
}

byte rngb() {
    return (byte)rng();
}
