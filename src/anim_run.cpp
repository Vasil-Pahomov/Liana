#include "effects.h"

// The palette cycles along the strip with a random speed and direction.
class EffectRun : public Effect {
    int pos = 0;
    int inc = 1;

public:
    void setUp() override {
        pos = 0;
        inc = 1 + (rngb() >> 5);
        if (random(10) > 5) {
            inc = -inc;
        }
    }

    void run() override {
        int p = pos;
        for (int i=0;i<ledsNum;i++) {
            Color c = palette->getPalColorCycle((float)p/256);
            leds[i] = c;

            p = p + inc;
            if (p >= 256) {
                p = p - 256;
            } else if (p < 0) {
                p = p + 256;
            }
        }
        pos = pos + 1;
        if (pos>255) pos = 0;
    }
};

Effect* createEffectRun() { return new EffectRun(); }
