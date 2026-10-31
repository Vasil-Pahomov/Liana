#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "config.h"

// Runtime settings, persisted as JSON in CONFIG_FILE_NAME on the flash file system.
// The initializers below are the defaults; they apply to keys missing in the file.
struct Settings {
    int leds = 100;          // number of LEDs in the strip, 1..MAXLEDS
    int brightness = 100;    // global brightness, 0..255
    int neofeature = 0;      // color order of the strip, see NeoPixelType in NeoPixelWrapper.h

    // bit masks of effects/palettes excluded from random cycling (bit set = excluded)
    uint16_t disabledAnims = 0;
    uint16_t disabledPals = 0;

    // MQTT client; empty host disables MQTT
    String mqttHost;
    int mqttPort = 0;
    String mqttClientId;
    String mqttLogin;
    String mqttPass;
    String mqttTopic;

    // night mode: a fixed effect with reduced brightness between nightStartHour and nightEndHour (local time)
    bool nightEnabled = (NIGHT_MODE_ENABLED != 0);
    int nightStartHour = NIGHT_START_HOUR; // 0..23
    int nightEndHour = NIGHT_END_HOUR;     // 0..23
    int nightAnim = NIGHT_ANIMATION_INDEX; // regular effect id
    int nightBrightness = NIGHT_BRIGHTNESS;

    // clamps all fields to valid ranges
    void validate();
    // reads the file; on any error keeps the defaults and returns false
    bool load();
    // validates and writes the file; returns false when the file could not be written
    bool save();

    int enabledAnimsCount() const;
    int enabledPalsCount() const;
    // special effects (magic, OTA, off) are always "enabled"
    bool isAnimEnabled(int id) const;
    bool isPalEnabled(int index) const;
    void setAnimEnabled(int id, bool enabled);
    void setPalEnabled(int index, bool enabled);

    // value of a setting by its JSON key, for {{key}} placeholders in pages; "" for unknown keys
    String get(const String& key) const;
};

extern Settings settings;

#endif
