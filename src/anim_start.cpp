#include "anim.h"
#include "config.h"

void Anim::animStart_SetUp() {
    Serial.print(F("(start)"));
    phase = 0;
}

void Anim::animStart_Run() {
    if (phase < ledsNum) {
        leds[phase].r = 255;
        leds[phase].g = 255;
        leds[phase].b = 255;
        for(int i=0; i<ledsNum; i++) {
            leds[i].fade(50);
        }
    } else if (phase >= ledsNum) 
    {
        int BRIGHTNESS = currentConfig.brightness;
        for(int i=0; i<ledsNum; i++) {
            int r = ledsNum + BRIGHTNESS - phase + rngb();
            r = min(r,BRIGHTNESS); leds[i].r = (byte)max(r,0);
            int g = ledsNum + BRIGHTNESS - phase + rngb();
            g = min(g,BRIGHTNESS); leds[i].g = (byte)max(g,0);
            int b = ledsNum + BRIGHTNESS - phase + rngb();
            b = min(b,BRIGHTNESS); leds[i].b = (byte)max(b,0);
        }
        phase++;
    }

    phase++;
}
