#include <WebSocketsServer.h>
//includes
#include <PersWiFiManager.h>
#include <ArduinoJson.h>
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
#include <EspHtmlTemplateProcessor.h>
#include <FileReader.h>

#include "palette.h"
#include "anim.h"
#include "config.h"
#include "web.h"
#include "websocket.h"
#include "wifi.h"
#include "mqtt.h"



void tests_runAll();

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

//#define USE_STATIC_BRB_AFTER_START // switch to static BRB after start animation for true Belarussians

unsigned long ms = 10000;//startup animation duration, 10000 default

void setup() { 
  Serial.begin(115200);
  //tests_runAll();

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
  while (!currentConfig.isPalEnabled(paletteInd) && paletteInd < PALS-1) { paletteInd++; };
  anim.setPalette(paletteInd);
  anim.doSetUp();

  Serial.println("Setup done");
}

void loop() {
  yield();
  
  anim.run();
  
  if (millis() > ms && animInd != 255) {// animind == 255 is for turned off strip - it never ends automatically
    ms = millis() + INTERVAL; 
    
#ifdef USE_START_ANIMATION  
    bool changeAnim = currentConfig.getEnabledAnimsCount() > 1;
    bool changePal = currentConfig.getEnabledPalsCount() > 1;
#else
    bool changeAnim = !currentConfig.isAnimEnabled(animInd) ||
                      ( currentConfig.isAnimEnabled(0)
                      ? (currentConfig.getEnabledAnimsCount() > 2)
                      : currentConfig.getEnabledAnimsCount() > 1 );
    bool changePal = (animInd > 0) && (!currentConfig.isPalEnabled(paletteInd) || (currentConfig.getEnabledPalsCount() > 1));
#endif

    if (changeAnim && changePal) {
      if (rngb() & 0x01) {
        changeAnim = false;
      } else {
        changePal = false;
      }
    }
    
    if (changeAnim) {
      Serial.print(F("anim->"));
      int prevAnimInd = animInd;
#ifdef USE_STATIC_BRB_AFTER_START
      if (animInd == 0) {
        paletteInd = 9; //BRB
        animInd = 9; //static
      } else
#endif
      {
        while ((prevAnimInd == animInd) 
          || !currentConfig.isAnimEnabled(animInd)) { 
#ifdef USE_START_ANIMATION
          animInd = random(ANIMS+1);
#else
          animInd = random(ANIMS) + 1; 
          //Serial.printf("%d-%d\r\n", animInd, currentConfig.isAnimEnabled(animInd));
  #endif
        }
        
        if (animInd == 0) { 
          ms = millis() + 10000;//startup animation has fixed 10 seconds length
        }

        setAnimPal();
        wsNotify(-1);
      }
    }
    if (changePal) {
      Serial.print(F("pal->"));
      int prevPalInd = paletteInd;
      while ((prevPalInd == paletteInd) 
        || !currentConfig.isPalEnabled(paletteInd)) {
        paletteInd = random(PALS);
        //Serial.printf("%d-%d\r\n", paletteInd, currentConfig.isPalEnabled(animInd));
      }

      anim.setPalette(paletteInd);
      Serial.print(paletteInd);
    }
  Serial.println();
  }
  /**/

  wifiLoop();
  mqttRun();

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

