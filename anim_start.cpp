#include "anim.h"

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
        for(int i=0; i<ledsNum; i++) {
            int r = ledsNum + 255 - phase + rngb();
            r = min(r,255); leds[i].r = (byte)max(r,0);
            int g = ledsNum + 255 - phase + rngb();
            g = min(g,255); leds[i].g = (byte)max(g,0);
            int b = ledsNum + 255 - phase + rngb();
            b = min(b,255); leds[i].b = (byte)max(b,0);
        }
        phase++;
    }

    phase++;
}
