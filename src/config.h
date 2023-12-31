#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME "/config.json"

// --------------
// Hardware config
// --------------
// Uncomment if you have button
//#define SUPPORT_BUTTON 1
#define BUTTON_ON_OFF_PIN 21

// Uncomment if you have button
//#define SUPPORT_LED_ON_OFF 1
#define LED_ON_OFF_PIN 2

#if defined(ESP8266)
#define LED_PIN 2
#elif defined(ESP32)
#define LED_PIN 14
#else
#error "Unsupported board class"
#endif

// --------------
// Animation config
// --------------
#define ANIMS 9 //number of animations to cycle randomly
#define PALS 11 //number of palettes
#define INTERVAL 30000 //change interval, msec

#define MAXLEDS 1024 // maximum number of LEDs in the strip TODO: switch to dynamic arrays to save RAM

#define TRANSITION_MS 1000 // transition time between animations, ms

#define ANIMATION_OTA_INDEX 101

// 0 - start animation, 255 - off
#define POWERON_ANIMATION_ID 0
#define ANIMATION_START_INDEX 0
#define ANIMATION_OFF_INDEX 255

// brigthness animation amplitude shift. true BrA amplitude is calculated as (0..127) value shifted right by this amount
#define BRA_AMP_SHIFT 1
// brigthness animation amplitude offset
#define BRA_OFFSET (222-64)

//probability of spark when in idle plase
#define SPARK_PROB 3

// --------------
// WiFi config
// --------------
#define WIFI_AP_SSID "Liana"
#define WIFI_CONNECTION_TIMEOUT 10000  //Timeout (in milliseconds) of waiting for WiFi connection
#define MDNS_NAME "liana"

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
