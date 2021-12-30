#include "anim.h"

void Anim::animStatic_SetUp() {
    Serial.print(F("(static)"));Serial.print(ledsNum);
    glowSetUp();
    inc = random(3)-1;
}


void Anim::animStatic_Run() {   
    for (int i=0;i<ledsNum;i++) {
        if (inc>0) {
            // bottom to top smooth
            leds[i] = palette->getPalColor((float)heightTransTable[i]/255.0); 
        } else if (inc<0) {
            // top to bottom smooth 
            leds[i] = palette->getPalColor(1.0-(float)heightTransTable[i]/255.0);
        } else {
            // bottom to top hard
            leds[i] = palette->colors[heightTransTable[i]*palette->numColors/255];
        }
        glowForEachLed(i);
    }

    glowRun();
    
    if (random(SPARK_PROB) == 0) {
        int i = (int)rngb() * ledsNum / 256;
        leds[i] = sparkleColor;
    }    
    
}
