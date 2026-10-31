#ifndef API_H
#define API_H

#include <ArduinoJson.h>

// JSON views shared by the HTTP API (/api/state, /api/catalog) and the WebSocket "state" message

// current player state: anim, pal, on, disabledAnims, disabledPals, brightness, night
void stateToJson(JsonObject obj);

// effects and palettes with ids, names, UI titles and palette colors
void catalogToJson(JsonObject obj);

#endif
