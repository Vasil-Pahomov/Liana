#include <FileReader.h>

#include "config.h"
#include "anim.h"

LianaConfig currentConfig;

void LianaConfig::configLoad() {
  File file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  Serial.println("Config is loading");
  if (deserializeJson(configJsonDoc, file)) {
    Serial.println("Config read error");
  } else {
    Serial.println("Config loaded");
  }
  file.close();
  leds = configJsonDoc["leds"];
  brightness = configJsonDoc["brightness"];
  neofeature = configJsonDoc["neofeature"];

  disabledAnims = configJsonDoc["disabledAnims"];
  disabledPals = configJsonDoc["disabledPals"];

  mqttHost = configJsonDoc["mqttHost"].as<String>();
  mqttPort = configJsonDoc["mqttPort"];
  mqttClientId = configJsonDoc["mqttClientId"].as<String>();
  mqttLogin = configJsonDoc["mqttLogin"].as<String>();
  mqttPass = configJsonDoc["mqttPass"].as<String>();
  mqttTopic = configJsonDoc["mqttTopic"].as<String>();

  if (leds > 1024 || leds <= 0) { leds = 100; } 
  if (brightness > 255 || brightness < 0) { brightness = 100; }
  if (neofeature > 255 || neofeature < 0) { neofeature = 0; }
}

void LianaConfig::configSave() {
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove(CONFIG_FILE_NAME);

  // Open file for writing
  File file = SPIFFS.open(CONFIG_FILE_NAME, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  configJsonDoc["leds"] = leds;
  configJsonDoc["brightness"] = brightness;
  configJsonDoc["neofeature"] = neofeature;

  configJsonDoc["disabledAnims"] = disabledAnims;
  configJsonDoc["disabledPals"] = disabledPals;

  configJsonDoc["mqttHost"] = mqttHost;
  configJsonDoc["mqttPort"] = mqttPort;
  configJsonDoc["mqttClientId"] = mqttClientId;
  configJsonDoc["mqttLogin"] = mqttLogin;
  configJsonDoc["mqttPass"] = mqttPass;
  configJsonDoc["mqttTopic"] = mqttTopic;
  

  if (serializeJson(configJsonDoc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();  
}

//counts zero bits
int zeroBitCount(uint16_t val, int maxBits) {
  int cnt = 0;
  for (int i=0;i<maxBits;i++, val>>=1) {
    if ((val & 0x0001) == 0) cnt++;
  }
  return cnt;
}

int LianaConfig::getEnabledAnimsCount() {
  return zeroBitCount(this->disabledAnims, ANIMS-1);
}

int LianaConfig::getEnabledPalsCount() {
  return zeroBitCount(this->disabledPals, PALS);
}


bool LianaConfig::isAnimEnabled(int animInd) {
  return !(this->disabledAnims & (1 << animInd));
}

void LianaConfig::setAnimEnabled(int animInd, bool isEnabled) {
  if (isEnabled) {
    this->disabledAnims &= ~(1U << animInd);
  } else {
    this->disabledAnims |= (1U << animInd);
  }
}

bool LianaConfig::isPalEnabled(int palInd) {
  return !(this->disabledPals & (1 << palInd));
}

void LianaConfig::setPalEnabled(int palInd, bool isEnabled) {
  if (isEnabled) {
    this->disabledPals &= ~(1U << palInd);
  } else {
    this->disabledPals |= (1U << palInd);
  }
}

uint16_t LianaConfig::getDisabledAnimsMask() {
  return this->disabledAnims;
}

uint16_t LianaConfig::getDisabledPalsMask() {
  return this->disabledPals;
}
