#include <PubSubClient.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "Unsupported board class"
#endif

#include "mqtt.h"
#include "player.h"
#include "effects.h"
#include "settings.h"
#include "liana_wifi.h"
#include "timeutil.h"

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

char mqttHost[100];

unsigned long lastConnAttemptMs;
#define MQTT_CONNECT_INTERVAL 10000

static bool startsWith(const char* s, const char* prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // PubSubClient does not terminate the payload
    char cmd[64];
    unsigned int n = length < sizeof(cmd) - 1 ? length : sizeof(cmd) - 1;
    memcpy(cmd, payload, n);
    cmd[n] = '\0';

    if (startsWith(cmd, "on")) {
        player.turnOn();
    } else
    if (startsWith(cmd, "off")) {
        player.turnOff();
    } else
    if (startsWith(cmd, "palette=")) {
        player.setPalette(atoi(cmd + 8));
    } else
    if (startsWith(cmd, "animation=")) {
        int animId = atoi(cmd + 10);
        if (isValidAnimIndex(animId)) {
            player.select(animId, player.currentPalette());
        }
    } else
    if (startsWith(cmd, "duration=")) {
        player.holdFor(atol(cmd + 9) * 1000L);
    } else
    if (startsWith(cmd, "brightness=")) {
        int bri = atoi(cmd + 11);
        if (bri >= 0 && bri <= 255) {
            player.setBrightness(bri);
        }
    } else
    if (startsWith(cmd, "brightness up")) {
        int bri = settings.brightness;
        bri = bri ? bri * 2 : 1;
        player.setBrightness(bri > 255 ? 255 : bri);
        Serial.printf("BRI=%d\n", settings.brightness);
    } else
    if (startsWith(cmd, "brightness down")) {
        player.setBrightness(settings.brightness / 2);
        Serial.printf("BRI=%d\n", settings.brightness);
    }
    else {
        Serial.printf("MQTT command unknown: length=%d, payload=%s\r\n", length, cmd);
    }
}

void mqttReconnect() {
    if (!timeReached(lastConnAttemptMs + MQTT_CONNECT_INTERVAL)) return;

    lastConnAttemptMs = millis();

    Serial.printf("Attempting MQTT connection to %s:%d, login=%s...",
        settings.mqttHost.c_str(), settings.mqttPort, settings.mqttLogin.c_str());

    if (pubSubClient.connect(settings.mqttClientId.c_str(), settings.mqttLogin.c_str(), settings.mqttPass.c_str())) {
        Serial.println(F("connected"));
        pubSubClient.subscribe(settings.mqttTopic.c_str());
    } else {
        Serial.print(F("failed, rc="));
        Serial.println(pubSubClient.state());
    }
}

void mqttSetup() {
    if (settings.mqttHost.length() > 0) {
        strlcpy(mqttHost, settings.mqttHost.c_str(), sizeof(mqttHost));
        pubSubClient.setServer(mqttHost, settings.mqttPort);
        pubSubClient.setCallback(mqttCallback);
    }
}

void mqttRun() {
    if (settings.mqttHost.length() > 0) {
        if (WiFi.status() == WL_CONNECTED && !pubSubClient.loop()) {
            mqttReconnect();
        }
    }
}
