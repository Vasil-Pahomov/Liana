#include "effects.h"

// Palette gradient along the "height" of a conic tree, with a glow and random sparks.
class EffectStatic : public Effect {
    int inc = 0; // >0 bottom to top smooth, <0 top to bottom smooth, 0 bottom to top hard
    Glow glow;

public:
    void setUp() override {
        glow.setUp();
        inc = random(3)-1;
    }

    void run() override {
        for (int i=0;i<ledsNum;i++) {
            Color c;
            if (inc>0) {
                c = palette->getPalColor((float)heightTransTable[i]/255.0);
            } else if (inc<0) {
                c = palette->getPalColor(1.0-(float)heightTransTable[i]/255.0);
            } else {
                c = palette->colors[heightTransTable[i]*palette->numColors/255];
            }
            leds[i] = glow.apply(c, i);
        }

        glow.advance();

        if (random(SPARK_PROB) == 0) {
            int i = (int)rngb() * ledsNum / 256;
            leds[i] = sparkleColor;
        }
    }
};

Effect* createEffectStatic() { return new EffectStatic(); }
