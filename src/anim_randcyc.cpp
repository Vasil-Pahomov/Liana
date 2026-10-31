#include "effects.h"

// Every LED cycles through the palette independently, with its own phase and speed.
class EffectRandCyc : public Effect {
public:
    void setUp() override {
        for (int i=0;i<ledsNum;i++) {
            seq[i] = rngb();
        }
    }

    void run() override {
        for (int i=0;i<ledsNum;i++) {
            leds[i] = palette->getPalColorCycle((float)seq[i] / 256);
            seq[i]+=rngb() >> 6;
        }
    }
};

Effect* createEffectRandCyc() { return new EffectRandCyc(); }
