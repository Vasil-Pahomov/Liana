#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME "/config.json"

#include <ArduinoJson.h>

class LianaConfig {
  private:
    void printConfig();

  public:
    LianaConfig() : leds(10),  brightness(255), configJsonDoc(1024) {}
       
    int leds;
    int brightness; // brightness adjustment, up to 256
    int neofeature; //NeoXXXFeature index

    //MQTT client settings
    String mqttHost;
    int mqttPort;
    String mqttClientId;
    String mqttLogin;
    String mqttPass;
    String mqttTopic;


    DynamicJsonDocument configJsonDoc;
  
    void configLoad();
  
    void configSave();
};

extern LianaConfig currentConfig;

#endif
