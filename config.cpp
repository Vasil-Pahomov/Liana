#include <FileReader.h>

#include "config.h"

void LianaConfig::configLoad() {
  File file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  if (deserializeJson(configJsonDoc, file)) {
    Serial.println("Config read error");
  }
  file.close();
  leds = configJsonDoc["leds"];
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

  if (serializeJson(configJsonDoc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();  

  file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  Serial.println("Saved config:");
  while (file.available()){
    Serial.println(file.readStringUntil('\n'));
  }
  file.close();

}
