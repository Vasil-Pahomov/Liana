#ifndef anim_h
#define anim_h
#include "palette.h"
#include "config.h"
#include "effects.h"

//TODO: it's better to hide these variables and make functions that accepts appropriate arguments
extern int paletteInd;
extern int animInd;

// Animation engine: owns the LED buffers, runs the current effect (and the outgoing one while
// crossfading), applies gamma and brightness and pushes the frame to the strip.
class Anim {
public:
    // allocates buffers for currentConfig.leds LEDs and initialises the strip; call once after the config is loaded
    void begin();

    // starts the effect with the given palette; the previous effect keeps running and fades out over TRANSITION_MS
    void select(int effectId, int palInd);

    // changes the palette of the running effect, restarting it with a crossfade if the effect requires that
    void setPalette(int palInd);

    // renders one frame once FRAME_MS has elapsed since the previous one
    void run();

    // input for the "magic" effect; ignored when it is not the current effect
    void setMagicParams(byte ind, byte position, byte color, bool isBoom);

private:
    struct Slot {
        Effect* effect = nullptr;
        const EffectInfo* info = nullptr;
        EffectContext ctx;
    };

    Slot slots[2];
    int cur = 0;                  // slots[cur] is the current effect, slots[1-cur] the outgoing one
    int ledsNum = 0;
    bool initialized = false;
    byte* heightTable = nullptr;

    unsigned long nextFrameMs = 0;
    unsigned long transEndMs = 0; // when the current crossfade ends
    bool transitioning = false;

    void dropEffect(Slot& s);
};

extern Anim anim;

//true if the index denotes an animation the user may select (regular, magic or off)
bool isValidAnimIndex(int animIndex);

void setAnimation(int animIndex);

void setPalette(int paletteIndex);

void setDuration(unsigned long durationMs);

void setAnimPal();

#endif
