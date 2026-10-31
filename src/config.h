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
#define ANIM_COUNT 10 // number of regular animations (indices 0..ANIM_COUNT-1); 0 is the start animation, 1..ANIM_COUNT-1 are cycled randomly
#define PALS 11 //number of palettes
#define INTERVAL 30000 //change interval, msec
#define START_ANIMATION_MS 10000 // the start animation has a fixed duration, msec

#define MAXLEDS 1024 // maximum number of LEDs in the strip TODO: switch to dynamic arrays to save RAM

#define FRAME_MS 20 // frame period, ms; all effects are tuned to it
#define TRANSITION_MS 1000 // crossfade time between animations, ms; both animations run during it

// special animation indices (outside 0..ANIM_COUNT-1)
#define ANIMATION_MAGIC_INDEX 100
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
#define WIFI_AP_SSID "Liana Long"
#define WIFI_CONNECTION_TIMEOUT 10000  //Timeout (in milliseconds) of waiting for WiFi connection
#define MDNS_NAME "liana"

// --------------
// Time and night mode config
// --------------
// POSIX timezone rule; applied without network access (this one is Europe/Warsaw)
#define TIMEZONE_POSIX "CET-1CEST,M3.5.0,M10.5.0/3"
// defaults for night mode; runtime values live in config.json
// (nightEnabled, nightStartHour, nightEndHour, nightAnim, nightBrightness)
#define NIGHT_MODE_ENABLED 1
#define NIGHT_START_HOUR 21
#define NIGHT_END_HOUR 6
#define NIGHT_ANIMATION_INDEX 5 // stars
#define NIGHT_BRIGHTNESS 50

#include <ArduinoJson.h>

class LianaConfig {
  public:
    LianaConfig() : leds(10), brightness(255), neofeature(0),
      mqttPort(0),
      nightEnabled(NIGHT_MODE_ENABLED != 0), nightStartHour(NIGHT_START_HOUR), nightEndHour(NIGHT_END_HOUR),
      nightAnim(NIGHT_ANIMATION_INDEX), nightBrightness(NIGHT_BRIGHTNESS),
      configJsonDoc(1024), disabledAnims(0), disabledPals(0) {}

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

    //night mode: at nighttime a fixed animation is played with reduced brightness
    bool nightEnabled;
    int nightStartHour; // 0..23, local time
    int nightEndHour;   // 0..23, local time
    int nightAnim;      // animation index to play at night
    int nightBrightness;

    DynamicJsonDocument configJsonDoc;

    void configLoad();

    void configSave();

    //clamps all fields to valid ranges
    void validate();

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
