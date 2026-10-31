#include "effects.h"

#define DUST_LENGTH 20

// A front of a new color runs along the strip with sparkling "dust" at its edge, on top of a glow.
class EffectPixieDust : public Effect {
    int phase = 0;
    int inc = 1;
    Color prevColor;
    Color curColor;
    Glow glow;

public:
    void setUp() override {
        phase = 0;
        prevColor = palette->getPalColor((float)rngb()/256);
        curColor = palette->getDifferentColor(prevColor);
        inc = random(2)*2-1;
        if (inc > 0) {
            phase = -DUST_LENGTH/2;
        } else {
            phase = ledsNum + DUST_LENGTH/2;
        }
        glow.setUp();
    }

    void run() override {
        if (inc > 0) {
            for (int i=0;i<ledsNum;i++) {
                leds[i] = glow.apply((i > phase) ? prevColor : curColor, i);
            }
            phase++;
            if (phase >= 4*ledsNum) {
                phase = -DUST_LENGTH/2;
                prevColor = curColor;
                curColor = palette->getDifferentColor(prevColor);
            }
        } else {
            for (int i=0;i<ledsNum;i++) {
                leds[i] = glow.apply((i < phase) ? prevColor : curColor, i);
            }
            phase--;
            if (phase <= -3*ledsNum) {
                phase = ledsNum + DUST_LENGTH/2;
                prevColor = curColor;
                curColor = palette->getDifferentColor(prevColor);
            }
        }
        glow.advance();

        for (int k = phase-DUST_LENGTH/2; k < (phase + DUST_LENGTH/2); k++ ) {
            if (k >= 0 && k < ledsNum) {
                int mix = abs(k-phase) * 255 / DUST_LENGTH + random(-100, 100);
                if (mix < 0) {
                    mix = 0;
                } else if (mix > 255) {
                    mix = 255;
                }
                Color sparkle = sparkleColor;
                leds[k] = sparkle.interpolate(leds[k], (float)mix/255);
            }
        }
    }
};

Effect* createEffectPixieDust() { return new EffectPixieDust(); }
