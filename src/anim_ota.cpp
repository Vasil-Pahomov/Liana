#include "anim.h"
#include "color.h"
#include "palette.h"
#include "ota.h"
#include "config.h"

void Anim::animOTA_SetUp() {
    for (int i=0;i<ledsNum;i++) {
        leds[i] = Color(0,0,0);
    }
}

void Anim::animOTA_Run() {
    int countLed = ledsNum * otaProgress / 100;
        int BRIGHTNESS = currentConfig.brightness;
    int startIndex = countLed - 2;
    if (startIndex < 0){
        startIndex = 0;
    }
    for(int i=startIndex; i<countLed; i++) {
        leds[i].r = 0;
        leds[i].g = BRIGHTNESS;
        leds[i].b = 0;
    }
    for(int i=0; i<startIndex; i++) {
        leds[i].fade(20);
    }
}