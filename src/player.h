#ifndef PLAYER_H
#define PLAYER_H

#include <Arduino.h>
#include <functional>
#include "config.h"

// What the UI and MQTT see
struct PlayerState {
    int anim;               // current effect id; ANIMATION_OFF_INDEX when the strip is off
    int palette;
    bool on;
    uint16_t disabledAnims; // effects excluded from random cycling
    uint16_t disabledPals;
    int brightness;
    bool nightActive;
};

// Owns "what is playing": the current effect and palette, the deadline of the next automatic change,
// random cycling and night mode. Every input (web, WebSocket, MQTT, button) goes through it,
// and every change is reported through onChange() so that WebSocket clients stay in sync.
class Player {
public:
    typedef std::function<void()> ChangeHandler;

    // starts the start animation with the first enabled palette; call after anim.begin()
    void begin();
    // automatic cycling and night mode; call from loop()
    void tick();

    // explicit selection (any registered effect id, a palette index); keeps playing for a full interval
    void select(int animId, int palInd);
    void setPalette(int palInd);
    void turnOn();
    void turnOff();
    bool isOn() const;

    // postpones the next automatic change by the given time
    void holdFor(unsigned long ms);

    // runtime brightness, not persisted
    void setBrightness(int brightness);

    // excluding effects/palettes from random cycling, persisted; the last enabled one cannot be disabled
    void setAnimEnabled(int id, bool enabled);
    void setPalEnabled(int index, bool enabled);

    PlayerState state() const;
    int currentAnim() const { return animInd; }
    int currentPalette() const { return paletteInd; }

    void onChange(ChangeHandler handler) { changeHandler = handler; }

private:
    int animInd = ANIMATION_START_INDEX;
    int paletteInd = 0;
    unsigned long deadline = 0;   // millis() of the next automatic change

    bool nightActive = false;
    int dayBrightness = 0;        // brightness to restore when the night is over

    ChangeHandler changeHandler;

    void apply();                 // pushes animInd/paletteInd to the engine and notifies
    void notify();
    void pickNext();              // changes either the effect or the palette at random
    void nightModeUpdate();
    bool isNightHour(int hour) const;
};

extern Player player;

#endif
