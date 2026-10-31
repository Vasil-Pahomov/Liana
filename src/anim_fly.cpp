#include "effects.h"

// Particles with a fading tail fly along the strip.
class EffectFly : public Effect {
    int pos = 2;   // length of the particle tail
    int inc = 5;   // (average) interval between particles; sign gives the direction
    int phase = 0; // remaining tail length of the current particle
    Color curColor;

public:
    void setUp() override {
        pos = random(2, 15);
        inc = random(5, 15);
        if (random(10) > 5) {
            inc = -inc;
        }
        phase = 0;
    }

    void run() override {
        int launchpos;
        if (inc > 0) {
            launchpos = ledsNum-1;
            for (int i=1;i<ledsNum;i++) {
                leds[i-1] = leds[i];
            }
        } else {
            launchpos = 0;
            for (int i=ledsNum-2;i>=0;i--) {
                leds[i+1] = leds[i];
            }
        }

        if (random(abs(inc)) == 0) {
            curColor = palette->getPalColor((float)rngb()/256);
            phase = pos;
        }

        leds[launchpos] = Color( (int)curColor.r * phase / pos, (int)curColor.g * phase / pos, (int)curColor.b * phase / pos) ;
        if (phase > 0) phase--;
    }
};

Effect* createEffectFly() { return new EffectFly(); }
