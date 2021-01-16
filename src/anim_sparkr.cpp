#include "anim.h"

void AnimSparkr_initSeq(byte * seq, int ledsNum)
{
    for (int i=0; i<ledsNum; i++) {
        seq[i] = i;
    }
}

void AnimSparkr_shuffleSeq(byte * seq, int ledsNum) 
{
    for (int i=0; i<ledsNum; i++) {
        byte ind = (unsigned int) ( rngb() * ledsNum / 256);
        if (ind != i) {
            byte tmp = seq[ind];
            seq[ind] = seq[i];
            seq[i] = tmp;
        }
    }
 
}

void Anim::animSparkr_SetUp() {
    Serial.print(F("(sparkr)"));
    glowSetUp();
    phase = 0;
    curColor = palette->getPalColor((float)rngb()/256);
    prevColor = palette->getPalColor((float)rngb()/256);
    AnimSparkr_initSeq(seq, ledsNum);
    AnimSparkr_shuffleSeq(seq, ledsNum);


}

void Anim::animSparkr_Run() {
    for (int i=0;i<ledsNum;i++) {
        byte pos = seq[i];

        leds[pos] = (i > phase)
            ? prevColor 
            : (i == phase) ? sparkleColor : curColor;
        glowForEachLed(pos);
    }
    glowRun();

    if (phase > ledsNum) {
        if (random(SPARK_PROB) == 0) {
            int i = (int)rngb() * ledsNum / 256;
            leds[i] = sparkleColor;

        }    
    }

    phase++;
    if (phase > 2*ledsNum) {
        phase = 0;
        prevColor = curColor;
        curColor = palette->getDifferentColor(prevColor);
        AnimSparkr_shuffleSeq(seq, ledsNum);
    }
}
