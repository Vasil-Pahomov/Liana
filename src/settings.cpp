#include <ArduinoJson.h>
#include "fs_compat.h"
#include "settings.h"
#include "palette.h"
#include "enable_mask.h"

Settings settings;

// integer value of a JSON field; older config files stored numbers as strings
static int intOr(JsonVariantConst v, int def) {
    if (v.is<int>()) return v.as<int>();
    if (v.is<const char*>()) {
        const char* s = v.as<const char*>();
        if (s != nullptr && *s != '\0') return atoi(s);
    }
    return def;
}

static String stringOr(JsonVariantConst v, const char* def) {
    const char* s = v | def;
    return String(s);
}

void Settings::validate() {
    if (leds > MAXLEDS || leds <= 0) leds = 100;
    if (brightness > 255 || brightness < 0) brightness = 100;
    if (neofeature > 255 || neofeature < 0) neofeature = 0;
    if (mqttPort < 0 || mqttPort > 65535) mqttPort = 0;
    if (nightStartHour < 0 || nightStartHour > 23) nightStartHour = NIGHT_START_HOUR;
    if (nightEndHour < 0 || nightEndHour > 23) nightEndHour = NIGHT_END_HOUR;
    if (nightAnim < 0 || nightAnim >= ANIM_COUNT) nightAnim = NIGHT_ANIMATION_INDEX;
    if (nightBrightness < 0 || nightBrightness > 255) nightBrightness = NIGHT_BRIGHTNESS;
}

bool Settings::load() {
    File file = LIANA_FS.open(CONFIG_FILE_NAME, "r");
    if (!file) {
        Serial.println(F("Config file not found, using defaults"));
        validate();
        return false;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
        Serial.print(F("Config read error: ")); Serial.println(error.f_str());
        validate();
        return false;
    }

    leds = intOr(doc["leds"], leds);
    brightness = intOr(doc["brightness"], brightness);
    neofeature = intOr(doc["neofeature"], neofeature);

    disabledAnims = (uint16_t)intOr(doc["disabledAnims"], disabledAnims);
    disabledPals = (uint16_t)intOr(doc["disabledPals"], disabledPals);

    mqttHost = stringOr(doc["mqttHost"], "");
    mqttPort = intOr(doc["mqttPort"], mqttPort);
    mqttClientId = stringOr(doc["mqttClientId"], "");
    mqttLogin = stringOr(doc["mqttLogin"], "");
    mqttPass = stringOr(doc["mqttPass"], "");
    mqttTopic = stringOr(doc["mqttTopic"], "");

    nightEnabled = intOr(doc["nightEnabled"], nightEnabled ? 1 : 0) != 0;
    nightStartHour = intOr(doc["nightStartHour"], nightStartHour);
    nightEndHour = intOr(doc["nightEndHour"], nightEndHour);
    nightAnim = intOr(doc["nightAnim"], nightAnim);
    nightBrightness = intOr(doc["nightBrightness"], nightBrightness);

    validate();
    Serial.println(F("Config loaded"));
    return true;
}

bool Settings::save() {
    validate();

    JsonDocument doc;
    doc["leds"] = leds;
    doc["brightness"] = brightness;
    doc["neofeature"] = neofeature;
    doc["disabledAnims"] = disabledAnims;
    doc["disabledPals"] = disabledPals;
    doc["mqttHost"] = mqttHost;
    doc["mqttPort"] = mqttPort;
    doc["mqttClientId"] = mqttClientId;
    doc["mqttLogin"] = mqttLogin;
    doc["mqttPass"] = mqttPass;
    doc["mqttTopic"] = mqttTopic;
    doc["nightEnabled"] = nightEnabled ? 1 : 0;
    doc["nightStartHour"] = nightStartHour;
    doc["nightEndHour"] = nightEndHour;
    doc["nightAnim"] = nightAnim;
    doc["nightBrightness"] = nightBrightness;

    // the file is recreated, otherwise the new content could be appended to the old one
    LIANA_FS.remove(CONFIG_FILE_NAME);
    File file = LIANA_FS.open(CONFIG_FILE_NAME, "w");
    if (!file) {
        Serial.println(F("Failed to create config file"));
        return false;
    }
    bool ok = serializeJson(doc, file) > 0;
    file.close();
    if (!ok) Serial.println(F("Failed to write config file"));
    return ok;
}

int Settings::enabledAnimsCount() const { return enabledCount(disabledAnims, ANIM_COUNT); }
int Settings::enabledPalsCount() const { return enabledCount(disabledPals, PALETTE_COUNT); }

bool Settings::isAnimEnabled(int id) const {
    if (id < 0 || id >= ANIM_COUNT) return true;
    return isEnabled(disabledAnims, id, ANIM_COUNT);
}

bool Settings::isPalEnabled(int index) const { return isEnabled(disabledPals, index, PALETTE_COUNT); }

void Settings::setAnimEnabled(int id, bool enabled) { disabledAnims = withEnabled(disabledAnims, id, ANIM_COUNT, enabled); }
void Settings::setPalEnabled(int index, bool enabled) { disabledPals = withEnabled(disabledPals, index, PALETTE_COUNT, enabled); }

String Settings::get(const String& key) const {
    if (key == "leds") return String(leds);
    if (key == "brightness") return String(brightness);
    if (key == "neofeature") return String(neofeature);
    if (key == "mqttHost") return mqttHost;
    if (key == "mqttPort") return mqttPort > 0 ? String(mqttPort) : String();
    if (key == "mqttClientId") return mqttClientId;
    if (key == "mqttLogin") return mqttLogin;
    if (key == "mqttPass") return mqttPass;
    if (key == "mqttTopic") return mqttTopic;
    if (key == "nightEnabled") return String(nightEnabled ? 1 : 0);
    if (key == "nightStartHour") return String(nightStartHour);
    if (key == "nightEndHour") return String(nightEndHour);
    if (key == "nightAnim") return String(nightAnim);
    if (key == "nightBrightness") return String(nightBrightness);
    return String();
}
