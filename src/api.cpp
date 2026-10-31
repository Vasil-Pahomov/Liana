#include "api.h"
#include "player.h"
#include "effects.h"
#include "palette.h"
#include "config.h"

void stateToJson(JsonObject obj)
{
    PlayerState s = player.state();
    obj["anim"] = s.anim;
    obj["pal"] = s.palette;
    obj["on"] = s.on;
    obj["disabledAnims"] = s.disabledAnims;
    obj["disabledPals"] = s.disabledPals;
    obj["brightness"] = s.brightness;
    obj["night"] = s.nightActive;
}

static const char* effectKind(int id)
{
    if (id == ANIMATION_MAGIC_INDEX) return "magic";
    if (id == ANIMATION_OTA_INDEX) return "ota";
    if (id == ANIMATION_OFF_INDEX) return "off";
    return "regular";
}

static String colorToHex(const Color& c)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X", c.r, c.g, c.b);
    return String(buf);
}

void catalogToJson(JsonObject obj)
{
    JsonArray effects = obj["effects"].to<JsonArray>();
    for (int i = 0; i < EFFECT_COUNT; i++) {
        const EffectInfo& info = EFFECTS[i];
        JsonObject e = effects.add<JsonObject>();
        e["id"] = info.id;
        e["name"] = info.name;
        e["title"] = info.title;
        e["kind"] = effectKind(info.id);
        // regular effects except the start one take part in random cycling and can be excluded by the user
        e["cyclable"] = (info.id > ANIMATION_START_INDEX && info.id < ANIM_COUNT);
    }

    JsonArray palettes = obj["palettes"].to<JsonArray>();
    for (int i = 0; i < PALETTE_COUNT; i++) {
        const PaletteInfo& info = PALETTES[i];
        JsonObject p = palettes.add<JsonObject>();
        p["id"] = i;
        p["name"] = info.name;
        p["title"] = info.title;
        JsonArray colors = p["colors"].to<JsonArray>();
        for (int c = 0; c < info.palette->numColors; c++) {
            colors.add(colorToHex(info.palette->colors[c]));
        }
    }
}
