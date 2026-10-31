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
#include <ezTime.h>

#include "palette.h"
#include "anim.h"
#include "config.h"
#include "web.h"
#include "websocket.h"
#include "liana_wifi.h"
#include "mqtt.h"
#include "ota.h"
#include "hw_button.h"
#include "hw_led.h"
#include "timeutil.h"


void tests_runAll();

//#define USE_START_ANIMATION //start animation is used in cycling as well as other animations

//#define USE_STATIC_BRB_AFTER_START // switch to static BRB after start animation for true Belarussians

unsigned long millisec = START_ANIMATION_MS; // deadline (millis) of the next automatic animation/palette change

Timezone tz;

// night mode state: true while the night animation and brightness are in effect
static bool nightActive = false;
// brightness to restore when the night is over
static int dayBrightness = 0;

static bool isNightHour(int hour) {
  int from = currentConfig.nightStartHour;
  int to = currentConfig.nightEndHour;
  if (from == to) return false;
  return (from < to) ? (hour >= from && hour < to) : (hour >= from || hour < to);
}

// switches night mode on/off according to the current local time
static void nightModeUpdate() {
  if (!currentConfig.nightEnabled || timeStatus() != timeSet) return;
  bool night = isNightHour(tz.hour());
  if (night == nightActive) return;
  nightActive = night;
  if (night) {
    dayBrightness = currentConfig.brightness;
    currentConfig.brightness = currentConfig.nightBrightness;
    Serial.println(F("Night mode on"));
  } else {
    currentConfig.brightness = dayBrightness;
    Serial.println(F("Night mode off"));
  }
}

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

  // a POSIX rule is applied locally; setLocation() would need the network, which is not up yet
  tz.setPosix(F(TIMEZONE_POSIX));

  randomSeed(analogRead(0)*analogRead(1));
  anim.begin();
  while (!currentConfig.isPalEnabled(paletteInd) && paletteInd < PALS-1) { paletteInd++; };
  setAnimPal();

  otaSetUp();
  
  #if defined(SUPPORT_LED_ON_OFF)
  ledSetUp();
  #endif

  #if defined(SUPPORT_BUTTON)
  buttonsSetUp();
  #endif
  Serial.println("Setup done");
}

void loop() {
  yield();
  
  anim.run();
  
  if (timeReached(millisec) && animInd != ANIMATION_OFF_INDEX) {// turned off strip never ends automatically
    millisec = millis() + INTERVAL;

    nightModeUpdate();

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
          animInd = random(ANIM_COUNT);
#else
          animInd = random(1, ANIM_COUNT);
          //Serial.printf("%d-%d\r\n", animInd, currentConfig.isAnimEnabled(animInd));
  #endif
        }

        //stick to the night animation at nighttime
        if (nightActive) {
          animInd = currentConfig.nightAnim;
        }

        if (animInd == ANIMATION_START_INDEX) {
          millisec = millis() + START_ANIMATION_MS;//startup animation has a fixed length
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
  otaLoop();
  events();
  #if defined(SUPPORT_BUTTON)
  buttonsLoop();
  #endif
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

