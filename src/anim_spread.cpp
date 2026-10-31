#include "effects.h"

// Colored spots appear at random places, spread out and fade.
class EffectSpread : public Effect {
    int inc = 2;   // (average) interval between new spots
    int phase = 1; // width of the "spread"

public:
    void setUp() override {
        inc = random(2,5);
        memset(seq, 0, ledsNum * sizeof(seq[0]));
        phase = ledsNum / 5;
        if (phase < 1) phase = 1;
    }

    void run() override {
        for (int i=0;i<ledsNum;i++) {
            leds[i].r = 0;
            leds[i].g = 0;
            leds[i].b = 0;
        }

        for (int i=0;i<ledsNum;i++) {
            if (seq[i] > 0) {
                int width = phase - seq[i];
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
            int pos = random(0,ledsNum);
            ledstmp[pos] = palette->getPalColor((float)rngb()/256);
            seq[pos] = phase;
        }
    }
};

Effect* createEffectSpread() { return new EffectSpread(); }
