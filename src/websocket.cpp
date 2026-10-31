#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include "websocket.h"
#include "anim.h"
#include "player.h"
#include "effects.h"
#include "palette.h"
#include "api.h"
#include "config.h"

WebSocketsServer webSocket(81);

/* Messages are JSON objects.
 * Server -> client:  {"type":"state", "anim":N, "pal":N, "on":bool, "disabledAnims":mask, "disabledPals":mask, "brightness":N, "night":bool}
 * Client -> server:  {"cmd":"info"}                                   - request the state; also postpones automatic cycling
 *                    {"cmd":"set", "anim":N, "pal":N}                  - select effect and palette (anim 255 = off)
 *                    {"cmd":"enable", "kind":"anim"|"pal", "index":N, "enabled":bool}
 *                    {"cmd":"point", "pos":0..255, "color":0..255, "boom":bool} - input for the "magic" effect
 */

//sends the current state to connection num, or to all connections if num is -1
void wsNotify(int num) {
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["type"] = "state";
  stateToJson(obj);
  String out;
  serializeJson(doc, out);
  if (num >= 0) {
    webSocket.sendTXT(num, out);
  } else {
    webSocket.broadcastTXT(out);
  }
}

static void wsHandleCommand(uint8_t num, JsonObjectConst msg) {
  const char* cmd = msg["cmd"] | "";

  if (strcmp(cmd, "info") == 0) {
    wsNotify(num);
    if (player.currentAnim() != ANIMATION_START_INDEX) {
      player.holdFor(INTERVAL); //"reset" duration for any animation except start
    }

  } else if (strcmp(cmd, "set") == 0) {
    int animId = msg["anim"] | -1;
    int palInd = msg["pal"] | player.currentPalette();
    if (!isValidAnimIndex(animId) || palInd < 0 || palInd >= PALETTE_COUNT) {
      Serial.printf("WS set rejected: anim=%d, pal=%d\n", animId, palInd);
      return;
    }
    player.select(animId, palInd);

  } else if (strcmp(cmd, "enable") == 0) {
    const char* kind = msg["kind"] | "";
    int index = msg["index"] | -1;
    bool enabled = msg["enabled"] | true;
    if (strcmp(kind, "anim") == 0) {
      player.setAnimEnabled(index, enabled);
    } else if (strcmp(kind, "pal") == 0) {
      player.setPalEnabled(index, enabled);
    }
    wsNotify(-1); //clients that sent a rejected change get the real state back

  } else if (strcmp(cmd, "point") == 0) {
    if (player.currentAnim() == ANIMATION_MAGIC_INDEX) {
      int pos = msg["pos"] | 0;
      int color = msg["color"] | 0;
      bool boom = msg["boom"] | false;
      anim.setMagicParams(num, (byte)pos, (byte)color, boom);
    }

  } else {
    Serial.printf("Unknown WS command: %s\n", cmd);
  }
}

void wsRun()
{
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT: {                   // if new text data is received
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, (const char*)payload, length);
        if (err) {
          Serial.printf("WS message is not JSON: %s\n", err.c_str());
        } else {
          wsHandleCommand(num, doc.as<JsonObjectConst>());
        }
      }
      break;
    default:
      Serial.printf("[%u] Unknown message type: %d", num, type);
      break;
  }
}

void wsSetup()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  player.onChange([]() { wsNotify(-1); });
}
