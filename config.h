#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME "/config.json"

#include <ArduinoJson.h>

class LianaConfig {
  private:
    void printConfig();
  public:
    LianaConfig() : configJsonDoc(1024),leds(10), brightness(255) {}
       
    int leds;
    int brightness; // brightness adjustment, up to 256
    int neofeature; //NeoXXXFeature index
  
    DynamicJsonDocument configJsonDoc;
  
    void configLoad();
  
    void configSave();
};
#endif
