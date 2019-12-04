#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME "/config.json"

#include <ArduinoJson.h>

class LianaConfig {
  public:
    LianaConfig() : configJsonDoc(1024) {}
       
    int leds;
  
    DynamicJsonDocument configJsonDoc;
  
    void configLoad();
  
    void configSave();
};
#endif
