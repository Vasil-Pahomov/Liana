#include "effects.h"
#include "settings.h"

// A white dot runs along the strip leaving a fading trail, then the strip fills with noise.
class EffectStart : public Effect {
    int phase = 0;

public:
    void setUp() override {
        phase = 0;
    }

    void run() override {
        if (phase < ledsNum) {
            leds[phase].r = 255;
            leds[phase].g = 255;
            leds[phase].b = 255;
            for(int i=0; i<ledsNum; i++) {
                leds[i].fade(50);
            }
        } else if (phase >= ledsNum)
        {
            int BRIGHTNESS = settings.brightness;
            for(int i=0; i<ledsNum; i++) {
                int r = ledsNum + BRIGHTNESS - phase + rngb();
                r = min(r,BRIGHTNESS); leds[i].r = (byte)max(r,0);
                int g = ledsNum + BRIGHTNESS - phase + rngb();
                g = min(g,BRIGHTNESS); leds[i].g = (byte)max(g,0);
                int b = ledsNum + BRIGHTNESS - phase + rngb();
                b = min(b,BRIGHTNESS); leds[i].b = (byte)max(b,0);
            }
            phase++;
        }

        phase++;
    }
};

Effect* createEffectStart() { return new EffectStart(); }
