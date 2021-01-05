#include "anim.h"
#include "color.h"
#include "palette.h"

void Anim::animSpread_SetUp() {
    Serial.print(F("(spread)"));
    inc = random(2,5);
    memset(seq, 0, ledsNum);
    phase = ledsNum / 5; // pos is width of the "spread"
}

void Anim::animSpread_Run() {
    memset(leds, 0, 3*ledsNum);

    for (int i=0;i<ledsNum;i++) {
        if (seq[i] > 0) {
            byte width = phase - seq[i];
            for (int j=i-width;j<=(i+width);j++) {
                Color c = ledstmp[i];
                if (j>=0 && j<ledsNum) {
                    leds[j].r += c.r;
                    leds[j].g += c.g;
                    leds[j].b += c.b;
                }
            }
            ledstmp[i].fade(255/phase);
            seq[i]--;
        }
    }

    if (random(inc) == 0) {
        byte pos = random(0,ledsNum); 
        ledstmp[pos] = palette->getPalColor((float)rngb()/256);
        seq[pos] = phase;
    }        
}
