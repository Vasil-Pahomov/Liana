#include "effects.h"
#include "settings.h"
#include "ota.h"

// Green progress bar of a firmware update.
class EffectOTA : public Effect {
public:
    void setUp() override {
        for (int i=0;i<ledsNum;i++) {
            leds[i] = Color(0,0,0);
        }
    }

    void run() override {
        int countLed = ledsNum * otaProgress / 100;
        int BRIGHTNESS = settings.brightness;
        int startIndex = countLed - 2;
        if (startIndex < 0){
            startIndex = 0;
        }
        for(int i=startIndex; i<countLed; i++) {
            leds[i].r = 0;
            leds[i].g = BRIGHTNESS;
            leds[i].b = 0;
        }
        for(int i=0; i<startIndex; i++) {
            leds[i].fade(20);
        }
    }
};

Effect* createEffectOTA() { return new EffectOTA(); }
