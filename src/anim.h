#ifndef anim_h
#define anim_h
#include "palette.h"

#include "config.h"

//TODO: it's better to hide these variables and make functions that accepts appropriate arguments 
extern int paletteInd;
extern int animInd;

class Anim {
    
private:
    bool initialized;

    int ledsNum;//actual number of LEDs
    

    //Color arrays - two for making transition
    static Color leds1[MAXLEDS];
    static Color leds2[MAXLEDS];
    //auxiliary colors array
    static Color ledstmp[MAXLEDS];

    void animStart();
    
    // length of animation timeslot (period)
    byte period;
    // array of Color to work with
    Color *leds;
    Palette *palette;

    // millis for next timeslot 
    unsigned long nextms;
    // millis to transition end
    unsigned long transms;

    //animation-specific variables
    int phase;
    int pos;
    int inc;

    //whether to call SetUp on palette change
    //(some animations require full transition with fade, otherwise the colors would change in a step, some not)
    bool setUpOnPalChange;

    //height translation table (used by some effects to linarly align data across NY tree assuming it's conic)
    byte * heightTransTable;

    Color curColor = Color(0);
    Color prevColor = Color(0);

    Color sparkleColor = Color(0xFFFFFF);

    static int seq[MAXLEDS];

    //brigthness animation (BrA) current initial phase
    byte braPhase;
    //braPhase change speed 
    byte braPhaseSpd=5;
    //BrA frequency (spatial)
    byte braFreq=150;

    //glow animation setup
    void glowSetUp();

    //glow animation - must be called for each LED after it's BASIC color is set
    //note this overwrites the LED color, so the glow assumes that color will be stored elsewhere (not in leds[])
    //or computed each time regardless previous leds[] value
    void glowForEachLed(int i);
    
    //glow animation - must be called at the end of each animaton run
    void glowRun();

    void setUp();

    //run and setup handlers
    void (Anim::*runImpl)();
    void (Anim::*setUpImpl)();


    //animation implementations
    void animOff_SetUp();
    void animOff_Run();

    
    void animStart_SetUp();
    void animStart_Run();

    void animRun_SetUp();
    void animRun_Run();
    
    void animPixieDust_SetUp();
    void animPixieDust_Run();
    
    void animSparkr_SetUp();
    void animSparkr_Run();

    void animRandCyc_SetUp();
    void animRandCyc_Run();

    void animStars_SetUp();
    void animStars_Run();

    void animSpread_SetUp();
    void animSpread_Run();

    void animFly_SetUp();
    void animFly_Run();

    void animPulse_SetUp();
    void animPulse_Run();

    void animStatic_SetUp();
    void animStatic_Run();

    void animMagic_SetUp();
    void animMagic_Run();

    void animOTA_SetUp();
    void animOTA_Run();
public:


    Anim();
    void setPeriod(byte period);
    void setPalette(int palind);
    void setAnim(byte animInd);
    void run();
    void doSetUp();
    void setMagicParams(byte ind, byte position, byte color, boolean isBoom);

};

extern Anim anim;

void setAnimation(int animIndex);

void setPalette(int paletteIndex);

void setDuration(unsigned long durationMs);

void setAnimPal();

#endif
