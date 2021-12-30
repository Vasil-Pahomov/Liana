#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME "/config.json"

#include <ArduinoJson.h>

class LianaConfig {
  public:
    LianaConfig() : leds(10),  brightness(255), configJsonDoc(1024) {}
       
    //TODO: hide these public fields under properties
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

    int getEnabledAnimsCount();

    int getEnabledPalsCount();

    bool isAnimEnabled(int animInd);

    void setAnimEnabled(int animInd, bool isEnabled);

    bool isPalEnabled(int palInd);

    void setPalEnabled(int palInd, bool isEnabled);

    uint16_t getDisabledAnimsMask();
    uint16_t getDisabledPalsMask();

  private:
    //bit masks for disabling animations and palettes; zero bit means effect/palette is enabled. All is enabled by default (zero values)
    int disabledAnims;
    int disabledPals; 
};

extern LianaConfig currentConfig;

#endif
