#include "effects.h"

void Effect::attach(const EffectContext& ctx) {
    leds = ctx.leds;
    ledstmp = ctx.tmp;
    seq = ctx.seq;
    ledsNum = ctx.ledsNum;
    palette = ctx.palette;
    heightTransTable = ctx.heightTable;
}

// Single source of truth for effect ids. The UI (data/index.htm) and the README list the same ids.
const EffectInfo EFFECTS[] = {
    { ANIMATION_START_INDEX, "start",     createEffectStart,     true  },
    { 1,                     "run",       createEffectRun,       true  },
    { 2,                     "pixiedust", createEffectPixieDust, true  },
    { 3,                     "sparkr",    createEffectSparkr,    true  },
    { 4,                     "randcyc",   createEffectRandCyc,   true  },
    { 5,                     "stars",     createEffectStars,     false },
    { 6,                     "spread",    createEffectSpread,    false },
    { 7,                     "fly",       createEffectFly,       false },
    { 8,                     "pulse",     createEffectPulse,     false },
    { 9,                     "static",    createEffectStatic,    false },
    { ANIMATION_MAGIC_INDEX, "magic",     createEffectMagic,     false },
    { ANIMATION_OTA_INDEX,   "ota",       createEffectOTA,       false },
    { ANIMATION_OFF_INDEX,   "off",       createEffectOff,       false },
};

const int EFFECT_COUNT = sizeof(EFFECTS) / sizeof(EFFECTS[0]);

const EffectInfo* findEffect(int id) {
    for (int i = 0; i < EFFECT_COUNT; i++) {
        if (EFFECTS[i].id == id) return &EFFECTS[i];
    }
    return nullptr;
}
