#include <NeoPixelBus.h>
#include "color.h"
#include "palette.h"
#include "anim.h"
#include "brightness.h"
#include "config.h"
#include "NeoPixelWrapper.h"
#include "math.h"
#include "hw_led.h"
#include "timeutil.h"

NeoPixelWrapper * strip;

Anim anim;

//TODO: hide inside the class, don't expose
int paletteInd = 0;
int animInd = POWERON_ANIMATION_ID;

//TODO: do something with this, it's bad thing to reference that var here
extern unsigned long millisec;

Palette * pals[PALS] = {&PalRgb, &PalRainbow, &PalRainbowStripe, &PalParty, &PalHeat, &PalFire, &PalIceBlue, &PalXMas, &PalNY, &PalBLR, &PalUA};

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
    ledsNum = currentConfig.leds;

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
        ctx.palette = pals[0];
        ctx.heightTable = heightTable;
    }

    strip = new NeoPixelWrapper();
    strip->Begin(currentConfig.neofeature, ledsNum, LED_PIN);

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
    if (palInd < 0 || palInd >= PALS) palInd = 0;

    // the outgoing slot may still hold an effect from an unfinished crossfade; it's dropped,
    // then the current effect becomes the outgoing one and the new effect takes the freed slot
    dropEffect(slots[1 - cur]);
    cur = 1 - cur;

    Slot& s = slots[cur];
    s.info = info;
    s.ctx.palette = pals[palInd];
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
    if (palInd < 0 || palInd >= PALS) return;
    Slot& s = slots[cur];
    if (s.effect == nullptr) return;
    if (s.info->restartOnPaletteChange) {
        select(s.info->id, palInd);
    } else {
        s.ctx.palette = pals[palInd];
        s.effect->setPalette(pals[palInd]);
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

    int brightness = currentConfig.brightness;
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

bool isValidAnimIndex(int animIndex)
{
    return (animIndex >= 0 && animIndex < ANIM_COUNT)
        || animIndex == ANIMATION_MAGIC_INDEX
        || animIndex == ANIMATION_OFF_INDEX;
}

//starts animation and palette specified in animInd and paletteInd (crossfading from the running one)
void setAnimPal() {
  anim.select(animInd, paletteInd);
}

//sets animation and resets "change" time
void setAnimation(int animIndex)
{
    if (!isValidAnimIndex(animIndex)) return;
    animInd = animIndex;
    setAnimPal();
    //startup animation has a fixed, shorter duration
    millisec = millis() + ((animInd == ANIMATION_START_INDEX) ? START_ANIMATION_MS : INTERVAL);
}

void setPalette(int paletteIndex)
{
    if (paletteIndex < 0 || paletteIndex >= PALS) return;
    paletteInd = paletteIndex;
    setAnimPal();
    millisec = millis() + INTERVAL;
}

void setDuration(unsigned long durationMs) {
    millisec = millis() + durationMs;
}
