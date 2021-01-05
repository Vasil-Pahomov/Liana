#include "anim.h"

void Anim::animStatic_SetUp() {
    Serial.print(F("(static)"));Serial.print(ledsNum);
    glowSetUp();
    inc = 0;//random(5)-2;
}


void Anim::animStatic_Run() {   
    for (int i=0;i<ledsNum;i++) {
        if (inc>0) {
            leds[i] = palette->getPalColor((float)heigthTransTable[i]/255.0); 
        } else if (inc<0) {
            leds[i] = palette->getPalColor((float)heigthTransTable[ledsNum-1-i]/255.0);
        } else {
            leds[i] = palette->colors[heigthTransTable[i]*palette->numColors/255];
        }
        glowForEachLed(i);
    }

    glowRun();
    
    if (random(SPARK_PROB) == 0) {
        int i = (int)rngb() * ledsNum / 256;
        leds[i] = sparkleColor;
    }    
    
}
