#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "anim.h"
#include "config.h"
#include "wifi.h"

WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqttReconnect() {
    if (wifiIsConnected() && !pubSubClient.connected()) {
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
            Serial.println("connected");
            Serial.println(pubSubClient.connected());
            pubSubClient.subscribe(currentConfig.mqttTopic.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.println(pubSubClient.state());
        }
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
        if (WiFi.status() == WL_CONNECTED && !pubSubClient.connected()) {
            mqttReconnect();
        } else {
            pubSubClient.loop();
        }
    }
}