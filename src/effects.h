#ifndef EFFECTS_H
#define EFFECTS_H

#include <Arduino.h>
#include "color.h"
#include "palette.h"
#include "config.h"

// Working buffers of one engine slot. An effect renders into them; two slots exist so that
// the outgoing and the incoming effect can run at the same time during a crossfade.
struct EffectContext {
    Color* leds = nullptr;            // output frame of this slot
    Color* tmp = nullptr;             // scratch colors, one per LED
    int* seq = nullptr;               // scratch ints, one per LED
    int ledsNum = 0;
    Palette* palette = nullptr;
    const byte* heightTable = nullptr; // 0..255 "height" of each LED on a conic tree; shared, read-only
};

// "Breathing" brightness modulation (BrA) used by several effects
struct Glow {
    byte phase = 0;
    byte phaseSpd = 5;
    byte freq = 150;

    // randomises speed and spatial frequency
    void setUp();
    // modulates the color of LED i
    Color apply(Color c, int i) const;
    // advances the phase; call once per frame
    void advance();
};

// Base class of all effects. An instance is created when the effect is selected and
// destroyed when it has faded out, so per-effect state lives in the instance.
class Effect {
public:
    virtual ~Effect() {}

    // binds the effect to the slot buffers; called once before setUp()
    void attach(const EffectContext& ctx);
    // the palette may be swapped while running (unless EffectInfo::restartOnPaletteChange)
    void setPalette(Palette* p) { palette = p; }

    virtual void setUp() = 0;
    virtual void run() = 0;

protected:
    Color* leds = nullptr;
    Color* ledstmp = nullptr;
    int* seq = nullptr;
    int ledsNum = 0;
    Palette* palette = nullptr;
    const byte* heightTransTable = nullptr;
    const Color sparkleColor = Color(0xFFFFFF);
};

// The "magic" effect is driven by device orientation sent over WebSocket, so it's public
#define MAGIC_MAX_SPOTS 5 // maximum number of spots (one per WebSocket client) to track
class EffectMagic : public Effect {
public:
    void setUp() override;
    void run() override;
    void setParams(byte ind, byte spotPosRel, byte spotCol, bool isBoom);
private:
    struct Spot {
        byte boomPhase;     // "boom" phase, 0 when idle
        byte color;         // spot color index
        int lb, ub;         // current lower and upper bounds of the spot
        int prevlb, prevub; // previous bounds
        int boomPos;        // boom position
    };
    Spot spots[MAGIC_MAX_SPOTS];
    int boomWidth = 0;
};

typedef Effect* (*EffectFactory)();

struct EffectInfo {
    uint8_t id;                  // index used by the UI, WebSocket, MQTT and the config masks
    const char* name;
    EffectFactory create;
    bool restartOnPaletteChange; // true: a palette change restarts the effect (with a crossfade)
};

// The registry. Regular effects have ids 0..ANIM_COUNT-1, special ones use ANIMATION_*_INDEX.
extern const EffectInfo EFFECTS[];
extern const int EFFECT_COUNT;

// nullptr when there is no effect with such id
const EffectInfo* findEffect(int id);

// factories, one per effect source file
Effect* createEffectStart();
Effect* createEffectRun();
Effect* createEffectPixieDust();
Effect* createEffectSparkr();
Effect* createEffectRandCyc();
Effect* createEffectStars();
Effect* createEffectSpread();
Effect* createEffectFly();
Effect* createEffectPulse();
Effect* createEffectStatic();
Effect* createEffectMagic();
Effect* createEffectOTA();
Effect* createEffectOff();

#endif
