#include "anim.h"
#include "color.h"
#include "palette.h"

#define DUST_LENGTH 20
void Anim::animPixieDust_SetUp() {
    Serial.print(F("(pixiedust)"));
    phase = 0;
    prevColor = palette->getPalColor((float)rng()/256);
    curColor = palette->getDifferentColor(prevColor);
    inc = random(2)*2-1;
    if (inc > 0) {
        phase = -DUST_LENGTH/2;
    } else {
        phase = ledsNum + DUST_LENGTH/2;
    }
    glowSetUp();
}

void Anim::animPixieDust_Run() {

    if (inc > 0) {
        for (int i=0;i<ledsNum;i++) {
            leds[i] = (i > phase) ? prevColor : curColor;
            glowForEachLed(i);
        }
        phase++;
        if (phase >= 4*ledsNum) {
            phase = -DUST_LENGTH/2;
            prevColor = curColor;
            curColor = palette->getDifferentColor(prevColor);     
        }
    } else {
        for (int i=0;i<ledsNum;i++) {
            leds[i] = (i < phase) ? prevColor : curColor;
            glowForEachLed(i);
        }
        phase--;
        if (phase <= -3*ledsNum) {
            phase = ledsNum + DUST_LENGTH/2;
            prevColor = curColor;
            curColor = palette->getDifferentColor(prevColor);
        }
    }
    glowRun();
    
    for (int k = phase-DUST_LENGTH/2; k < (phase + DUST_LENGTH/2); k++ ) {
        if (k >= 0 && k < ledsNum) {
            int mix = abs(k-phase) * 255 / DUST_LENGTH + random(-100, 100);
            if (mix < 0) { 
                mix = 0;
            } else if (mix > 255) {
                mix = 255;
            }
            leds[k] = sparkleColor.interpolate(leds[k], (float)mix/255);
        }
    }
    
}
