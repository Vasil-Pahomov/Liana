#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PersWiFiManager.h>
#include <EspHtmlTemplateProcessor.h>
#include <FileReader.h>
#include <ArduinoJson.h>

#include "palette.h"
#include "anim.h"
#include "config.h"
#include "web.h"
#include "websocket.h"
#include "wifi.h"
#include "mqtt.h"

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

//#define USE_STATIC_BRB_AFTER_START // switch to static BRB after start animation

unsigned long ms = 10000;//startup animation duration, 10000 for "release" AnimStart

void setup() { 
  Serial.begin(115200);
  Serial.println("Entering setup");
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS started");
  } else {
    Serial.println("SPIFFS FAILED");
  }
  currentConfig.configLoad();
  
  wifiSetUp();
  mqttSetup();

  randomSeed(analogRead(0)*analogRead(1));
  anim.setAnim(animInd);
  anim.setPeriod(20);
  anim.setPalette(0);
  anim.doSetUp();

  Serial.println("Setup done");
}


void loop() {
  yield();
  
  /* this piece of code checks for looping while trying to find different colors
  for (int pi=0;pi<PALS;pi++) {
    int c = 0;
    
    Serial.print(F("pi="));Serial.print(pi);
    Color c1 = pals[pi]->getPalColor((float)rngb()/256);
    Color c2 = c1;
    while (c1.isCloseTo(c2)) {
      c = c + 1;
      c2 = pals[pi]->getPalColor((float)rngb()/256);
    }
    Serial.print(F(" c="));Serial.println(c);
  }
  /**/

  anim.run();
  
  if (millis() > ms && animInd != 255) {// animind == 255 is for turned off strip - it never ends automatically
    ms = millis() + INTERVAL; 
    switch ( (animInd <= 0) ? 0 : random(2)) {
      case 0: 
      {
        Serial.print(F("anim->"));
        int prevAnimInd = animInd;
#ifdef USE_STATIC_BRB_AFTER_START
        if (animInd == 0) {
          paletteInd = 8; //BRB
          animInd = 9; //static
        } else
#endif
        {
#ifdef USE_START_ANIMATION
        while (prevAnimInd == animInd) animInd = random(ANIMS+1);
        if (animInd == 0) ms = millis() + 10000;//startup animation has fixed 10 seconds length
#else
        while (prevAnimInd == animInd) animInd = random(ANIMS) + 1;
#endif   
        }     
        setAnimPal();
        wsNotify(-1);
        break;
      }
      case 1:
      {
        Serial.print(F("pal->"));
        int prevPalInd = paletteInd;
        while (prevPalInd == paletteInd) paletteInd = random(PALS);
        anim.setPalette(paletteInd);
        Serial.print(paletteInd);
        break;
      }
    }
    Serial.println();
  }
  /**/

  wifiLoop();
  mqttRun();

}

