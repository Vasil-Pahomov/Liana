#include "effects.h"

// All LEDs off. The crossfade in the engine makes the strip fade to black.
class EffectOff : public Effect {
public:
    void setUp() override {
        for (int i=0;i<ledsNum;i++) {
            leds[i] = Color(0,0,0);
        }
    }

    void run() override {
    }
};

Effect* createEffectOff() { return new EffectOff(); }
