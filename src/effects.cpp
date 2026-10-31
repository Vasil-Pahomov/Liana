#include "effects.h"

void Effect::attach(const EffectContext& ctx) {
    leds = ctx.leds;
    ledstmp = ctx.tmp;
    seq = ctx.seq;
    ledsNum = ctx.ledsNum;
    palette = ctx.palette;
    heightTransTable = ctx.heightTable;
}

// Single source of truth for effect ids, names and UI labels. The UI builds its lists from /api/catalog.
const EffectInfo EFFECTS[] = {
    { ANIMATION_START_INDEX, "start",     "Начальная", createEffectStart,     true  },
    { 1,                     "run",       "Бег",       createEffectRun,       true  },
    { 2,                     "pixiedust", "Пыльца",    createEffectPixieDust, true  },
    { 3,                     "sparkr",    "Вспышки",   createEffectSparkr,    true  },
    { 4,                     "randcyc",   "Мерцание",  createEffectRandCyc,   true  },
    { 5,                     "stars",     "Звёзды",    createEffectStars,     false },
    { 6,                     "spread",    "Полосы",    createEffectSpread,    false },
    { 7,                     "fly",       "Полёт",     createEffectFly,       false },
    { 8,                     "pulse",     "Пульс",     createEffectPulse,     false },
    { 9,                     "static",    "Статика",   createEffectStatic,    false },
    { ANIMATION_MAGIC_INDEX, "magic",     "Магия",     createEffectMagic,     false },
    { ANIMATION_OTA_INDEX,   "ota",       "OTA",       createEffectOTA,       false },
    { ANIMATION_OFF_INDEX,   "off",       "Выкл",      createEffectOff,       false },
};

const int EFFECT_COUNT = sizeof(EFFECTS) / sizeof(EFFECTS[0]);

const EffectInfo* findEffect(int id) {
    for (int i = 0; i < EFFECT_COUNT; i++) {
        if (EFFECTS[i].id == id) return &EFFECTS[i];
    }
    return nullptr;
}

bool isValidAnimIndex(int animIndex)
{
    return animIndex != ANIMATION_OTA_INDEX && findEffect(animIndex) != nullptr;
}
