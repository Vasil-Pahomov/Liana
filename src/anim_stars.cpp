#include "effects.h"

// Random LEDs smoothly light up and fade out.
// seq keeps phases: 0..127 increasing, 128..255 decreasing, ends at 255 (steady off)
// ledstmp keeps color of stars
class EffectStars : public Effect {
    int inc = 0; // (average) interval between appearance of new stars

public:
    void setUp() override {
        inc = 10 - ledsNum / 20;
        if (inc < 0) inc=0;

        //reset all phases
        for (int i=0;i<ledsNum;i++) {
            seq[i] = 255;
        }
    }

    void run() override {
        for (int i=0;i<ledsNum;i++) {
            byte phi = seq[i];
            if (phi < 254) {
                Color col = ledstmp[i];
                if (phi <= 127) {
                    leds[i] = col.brightness(phi << 1);
                } else {
                    leds[i] = col.brightness((255-phi) << 1);
                }
                seq[i]+=2;
            } else {
                leds[i].r = 0;
                leds[i].g = 0;
                leds[i].b = 0;
            }
        }

        if (random(inc) == 0) {
            int pos = random(ledsNum);
            if (seq[pos] > 250) {
                seq[pos] = 0;
                ledstmp[pos] = palette->getPalColor((float)rngb()/256);
            }
        }
    }
};

Effect* createEffectStars() { return new EffectStars(); }
