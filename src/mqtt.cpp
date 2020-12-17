#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "anim.h"
#include "config.h"
#include "wifi.h"

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

unsigned long lastConnAttemptMs;
#define MQTT_CONNECT_INTERVAL 10000

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    if (strstr((char*)payload, "on") == (char*)payload) {
        setAnimation(0);
    } else
    if (strstr((char*)payload, "off") == (char*)payload) {
        setAnimation(255);//TODO: explicit method for turning off, not using magic numbers
    } else
    if (strstr((char*)payload, "palette=") == (char*)payload) {
        setPalette(atoi((char*) (payload+8)));
    } else 
    if (strstr((char*)payload, "animation=") == (char*)payload) {
        setAnimation(atoi((char*) (payload+10)));
    }
    if (strstr((char*)payload, "duration=") == (char*)payload) {
        setDuration(atol((char*)(payload+9))*1000L);
    }
    if (strstr((char*)payload, "brigthness=") == (char*)payload) {
        int bri = atoi((char*)(payload+11));
        if (bri >= 0 && bri <= 255) {
            currentConfig.brightness = bri;
        }
    }
    if (strstr((char*)payload, "brigthness up") == (char*)payload) {
        if (currentConfig.brightness) {
            currentConfig.brightness *= 2;
            if (currentConfig.brightness > 255) {
                currentConfig.brightness = 255;
            }
        } else {
            currentConfig.brightness = 1;
        }
        Serial.printf("BRI=%d\n", currentConfig.brightness);
    } else
    if (strstr((char*)payload, "brigthness down") == (char*)payload) {
        currentConfig.brightness /= 2;
        Serial.printf("BRI=%d\n", currentConfig.brightness);
    }
    else {
        Serial.print("MQTT command unknown [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
    }
}

void mqttReconnect() {
    if (millis() < (lastConnAttemptMs + MQTT_CONNECT_INTERVAL)) return;
    
    lastConnAttemptMs = millis();

    Serial.print(pubSubClient.connected());
    Serial.print("Attempting MQTT connection to ");
    Serial.print(currentConfig.mqttHost);
    Serial.print(':');
    Serial.print(currentConfig.mqttPort);
    Serial.print(", login:");
    Serial.print(currentConfig.mqttLogin.c_str());
    Serial.print(", pass:");
    Serial.print(currentConfig.mqttPass.c_str());
    Serial.print('-');

    if (pubSubClient.connect(currentConfig.mqttClientId.c_str(), currentConfig.mqttLogin.c_str(), currentConfig.mqttPass.c_str())) {
        Serial.print(pubSubClient.connected());
        Serial.println("connected");
        pubSubClient.subscribe(currentConfig.mqttTopic.c_str());
    } else {
        Serial.print("failed, rc=");
        Serial.println(pubSubClient.state());
    }
}

void mqttSetup() {
    if (currentConfig.mqttHost.length() > 0) {
        pubSubClient.setServer(currentConfig.mqttHost.c_str(), currentConfig.mqttPort);
        pubSubClient.setCallback(mqttCallback);
    } else {
    }
}

void mqttRun() {
    if (currentConfig.mqttHost.length() > 0) {
        if (WiFi.status() == WL_CONNECTED && !pubSubClient.loop()) {
            mqttReconnect();
        } 
    }
}