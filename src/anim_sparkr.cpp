#include "effects.h"

static void sparkrInitSeq(int * seq, int ledsNum)
{
    for (int i=0; i<ledsNum; i++) {
        seq[i] = i;
    }
}

static void sparkrShuffleSeq(int * seq, int ledsNum)
{
    for (int i=0; i<ledsNum; i++) {
        int ind = (unsigned int) ( rngb() * ledsNum / 256);
        if (ind != i) {
            int tmp = seq[ind];
            seq[ind] = seq[i];
            seq[i] = tmp;
        }
    }
}

// LEDs switch to a new color one by one in a random order with a white spark; random sparks while idle.
class EffectSparkr : public Effect {
    int phase = 0;
    Color curColor;
    Color prevColor;
    Glow glow;

public:
    void setUp() override {
        glow.setUp();
        phase = 0;
        curColor = palette->getPalColor((float)rngb()/256);
        prevColor = palette->getPalColor((float)rngb()/256);
        sparkrInitSeq(seq, ledsNum);
        sparkrShuffleSeq(seq, ledsNum);
    }

    void run() override {
        for (int i=0;i<ledsNum;i++) {
            int pos = seq[i];

            Color c = (i > phase)
                ? prevColor
                : (i == phase) ? sparkleColor : curColor;
            leds[pos] = glow.apply(c, pos);
        }
        glow.advance();

        if (phase > ledsNum) {
            if (random(SPARK_PROB) == 0) {
                int i = (int)rngb() * ledsNum / 256;
                leds[i] = sparkleColor;
            }
        }

        phase++;
        if (phase > 2*ledsNum) {
            phase = 0;
            prevColor = curColor;
            curColor = palette->getDifferentColor(prevColor);
            sparkrShuffleSeq(seq, ledsNum);
        }
    }
};

Effect* createEffectSparkr() { return new EffectSparkr(); }
