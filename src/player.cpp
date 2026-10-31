#include "player.h"
#include <ezTime.h>
#include "anim.h"
#include "settings.h"
#include "effects.h"
#include "palette.h"
#include "cycling.h"
#include "timeutil.h"

Player player;

extern Timezone tz;

static int arduinoRandom(int from, int to) {
    return (int)random(from, to);
}

void Player::begin()
{
    animInd = ANIMATION_START_INDEX;
    paletteInd = 0;
    while (!settings.isPalEnabled(paletteInd) && paletteInd < PALETTE_COUNT - 1) {
        paletteInd++;
    }
    deadline = millis() + START_ANIMATION_MS;
    apply();
}

void Player::apply()
{
    anim.select(animInd, paletteInd);
    notify();
}

void Player::notify()
{
    if (changeHandler) changeHandler();
}

PlayerState Player::state() const
{
    PlayerState s;
    s.anim = animInd;
    s.palette = paletteInd;
    s.on = isOn();
    s.disabledAnims = settings.disabledAnims;
    s.disabledPals = settings.disabledPals;
    s.brightness = settings.brightness;
    s.nightActive = nightActive;
    return s;
}

bool Player::isOn() const
{
    return animInd != ANIMATION_OFF_INDEX;
}

void Player::select(int animId, int palInd)
{
    if (findEffect(animId) == nullptr) return;
    if (palInd < 0 || palInd >= PALETTE_COUNT) return;
    animInd = animId;
    paletteInd = palInd;
    //startup animation has a fixed, shorter duration
    deadline = millis() + ((animInd == ANIMATION_START_INDEX) ? START_ANIMATION_MS : INTERVAL);
    apply();
}

void Player::setPalette(int palInd)
{
    if (palInd < 0 || palInd >= PALETTE_COUNT) return;
    paletteInd = palInd;
    deadline = millis() + INTERVAL;
    apply();
}

void Player::turnOn()
{
    select(ANIMATION_START_INDEX, paletteInd);
}

void Player::turnOff()
{
    select(ANIMATION_OFF_INDEX, paletteInd);
}

void Player::holdFor(unsigned long ms)
{
    deadline = millis() + ms;
}

void Player::setBrightness(int brightness)
{
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    settings.brightness = brightness;
    notify();
}

void Player::setAnimEnabled(int id, bool enabled)
{
    if (id < 0 || id >= ANIM_COUNT) return;
    // the start animation does not count: cycling needs one more enabled effect besides it
    int minEnabled = (settings.isAnimEnabled(ANIMATION_START_INDEX) && id != ANIMATION_START_INDEX) ? 2 : 1;
    if (!enabled && settings.enabledAnimsCount() <= minEnabled) return;
    settings.setAnimEnabled(id, enabled);
    settings.save();
    notify();
}

void Player::setPalEnabled(int index, bool enabled)
{
    if (index < 0 || index >= PALETTE_COUNT) return;
    if (!enabled && settings.enabledPalsCount() <= 1) return;
    settings.setPalEnabled(index, enabled);
    settings.save();
    notify();
}

bool Player::isNightHour(int hour) const
{
    int from = settings.nightStartHour;
    int to = settings.nightEndHour;
    if (from == to) return false;
    return (from < to) ? (hour >= from && hour < to) : (hour >= from || hour < to);
}

// switches night mode on/off according to the current local time
void Player::nightModeUpdate()
{
    if (!settings.nightEnabled || timeStatus() != timeSet) return;
    bool night = isNightHour(tz.hour());
    if (night == nightActive) return;
    nightActive = night;
    if (night) {
        dayBrightness = settings.brightness;
        settings.brightness = settings.nightBrightness;
        Serial.println(F("Night mode on"));
    } else {
        settings.brightness = dayBrightness;
        Serial.println(F("Night mode off"));
    }
    notify();
}

void Player::tick()
{
    // the strip that is off or being updated never changes by itself
    if (animInd == ANIMATION_OFF_INDEX || animInd == ANIMATION_OTA_INDEX) return;
    if (!timeReached(deadline)) return;
    deadline = millis() + INTERVAL;

    nightModeUpdate();
    pickNext();
}

// Either the effect or the palette changes, never both at once. Disabled items are skipped;
// the start animation (0) never comes up by itself.
void Player::pickNext()
{
    int enabledAnims = settings.enabledAnimsCount();
    bool changeAnim = !settings.isAnimEnabled(animInd)
        || (settings.isAnimEnabled(ANIMATION_START_INDEX) ? enabledAnims > 2 : enabledAnims > 1);
    bool changePal = (animInd != ANIMATION_START_INDEX)
        && (!settings.isPalEnabled(paletteInd) || settings.enabledPalsCount() > 1);

    if (changeAnim && changePal) {
        if (rngb() & 0x01) {
            changeAnim = false;
        } else {
            changePal = false;
        }
    }

    if (changeAnim) {
        int next = pickRandomEnabled(settings.disabledAnims, ANIM_COUNT, 1, ANIM_COUNT, animInd, arduinoRandom);
        if (nightActive) {
            next = settings.nightAnim; // stick to the night animation at nighttime
        }
        if (next >= 0) {
            Serial.print(F("anim->"));
            animInd = next;
            if (animInd == ANIMATION_START_INDEX) {
                deadline = millis() + START_ANIMATION_MS;
            }
            apply();
        }
    }
    if (changePal) {
        int next = pickRandomEnabled(settings.disabledPals, PALETTE_COUNT, 0, PALETTE_COUNT, paletteInd, arduinoRandom);
        if (next >= 0) {
            Serial.print(F("pal->")); Serial.print(next);
            paletteInd = next;
            anim.setPalette(paletteInd);
            notify();
        }
    }
    Serial.println();
}
