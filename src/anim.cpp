#include <NeoPixelBus.h>
#include "color.h"
#include "palette.h"
#include "anim.h"
#include "brightness.h"
#include "config.h"
#include "settings.h"
#include "NeoPixelWrapper.h"
#include "math.h"
#include "hw_led.h"
#include "timeutil.h"

NeoPixelWrapper * strip;

Anim anim;

// gamma correction (BRI table) and global brightness
static inline RgbColor toStripColor(const Color& c, int brightness) {
    byte r = (int)pgm_read_byte_near(BRI + c.r) * brightness / 256;
    byte g = (int)pgm_read_byte_near(BRI + c.g) * brightness / 256;
    byte b = (int)pgm_read_byte_near(BRI + c.b) * brightness / 256;
    return RgbColor(r, g, b);
}

void Anim::begin()
{
    if (initialized) return;
    ledsNum = settings.leds;

    // the registry must provide every regular effect id, otherwise cycling would select nothing
    for (int id = 0; id < ANIM_COUNT; id++) {
        if (findEffect(id) == nullptr) {
            Serial.printf("Effect %d is missing in the registry\n", id);
        }
    }

    // height translation table (used by some effects to linearly align data across a NY tree assuming it's conic)
    heightTable = new byte[ledsNum];
    for (int i = 0; i < ledsNum; i++) {
        heightTable[i] = (byte)floorf(255.0 * (1.0 - sqrtf(1.0 - (float)i / (float)ledsNum)));
    }

    for (int s = 0; s < 2; s++) {
        EffectContext& ctx = slots[s].ctx;
        ctx.leds = new Color[ledsNum];
        ctx.tmp = new Color[ledsNum];
        ctx.seq = new int[ledsNum]();
        ctx.ledsNum = ledsNum;
        ctx.palette = PALETTES[0].palette;
        ctx.heightTable = heightTable;
    }

    strip = new NeoPixelWrapper();
    strip->Begin(settings.neofeature, ledsNum, LED_PIN);

    nextFrameMs = millis();
    initialized = true;
}

void Anim::dropEffect(Slot& s)
{
    delete s.effect;
    s.effect = nullptr;
    s.info = nullptr;
}

void Anim::select(int effectId, int palInd)
{
    if (!initialized) return;
    const EffectInfo* info = findEffect(effectId);
    if (info == nullptr) info = findEffect(ANIMATION_OFF_INDEX);
    if (palInd < 0 || palInd >= PALETTE_COUNT) palInd = 0;

    // the outgoing slot may still hold an effect from an unfinished crossfade; it's dropped,
    // then the current effect becomes the outgoing one and the new effect takes the freed slot
    dropEffect(slots[1 - cur]);
    cur = 1 - cur;

    Slot& s = slots[cur];
    s.info = info;
    s.ctx.palette = PALETTES[palInd].palette;
    for (int i = 0; i < ledsNum; i++) {
        s.ctx.leds[i] = Color(0);
    }
    s.effect = info->create();
    s.effect->attach(s.ctx);
    s.effect->setUp();

    transitioning = (slots[1 - cur].effect != nullptr);
    transEndMs = millis() + TRANSITION_MS;

    if (effectId == ANIMATION_OFF_INDEX) {
        ledTurnOff();
    } else {
        ledTurnOn();
    }
    Serial.print('('); Serial.print(info->name); Serial.print(')');
}

void Anim::setPalette(int palInd)
{
    if (palInd < 0 || palInd >= PALETTE_COUNT) return;
    Slot& s = slots[cur];
    if (s.effect == nullptr) return;
    if (s.info->restartOnPaletteChange) {
        select(s.info->id, palInd);
    } else {
        s.ctx.palette = PALETTES[palInd].palette;
        s.effect->setPalette(s.ctx.palette);
    }
}

void Anim::run()
{
    if (!initialized || !timeReached(nextFrameMs)) {
        return;
    }
    nextFrameMs = millis() + FRAME_MS;

    Slot& c = slots[cur];
    Slot& o = slots[1 - cur];

    if (c.effect != nullptr) {
        c.effect->run();
    }

    // crossfade coefficient: 1 at the start of a transition, 0 at its end
    float mix = 0;
    if (transitioning) {
        mix = (float)(long)(transEndMs - millis()) / TRANSITION_MS;
        if (mix <= 0 || o.effect == nullptr) {
            transitioning = false;
            dropEffect(o);
            mix = 0;
        }
    }

    int brightness = settings.brightness;
    if (mix > 0) {
        // the outgoing effect keeps animating while it fades out
        o.effect->run();
        for (int i = 0; i < ledsNum; i++) {
            Color col = c.ctx.leds[i].interpolate(o.ctx.leds[i], mix);
            strip->SetPixelColor(i, toStripColor(col, brightness));
        }
    } else {
        for (int i = 0; i < ledsNum; i++) {
            strip->SetPixelColor(i, toStripColor(c.ctx.leds[i], brightness));
        }
    }

    strip->Show();
}

void Anim::setMagicParams(byte ind, byte position, byte color, bool isBoom)
{
    Slot& s = slots[cur];
    if (s.effect != nullptr && s.info->id == ANIMATION_MAGIC_INDEX) {
        static_cast<EffectMagic*>(s.effect)->setParams(ind, position, color, isBoom);
    }
}
