#ifndef CONFIG_H
#define CONFIG_H

// Compile-time configuration. Every value guarded by #ifndef can be overridden with a build flag,
// e.g. build_flags = -DSUPPORT_BUTTON=1 -DBUTTON_ON_OFF_PIN=5 in platformio_local.ini.
// Runtime settings (number of LEDs, brightness, MQTT, night mode) live in settings.h / config.json.

#define CONFIG_FILE_NAME "/config.json"

// --------------
// Hardware config
// --------------
// -DSUPPORT_BUTTON=1 if you have an on/off button
#ifndef BUTTON_ON_OFF_PIN
#define BUTTON_ON_OFF_PIN 21
#endif

// -DSUPPORT_LED_ON_OFF=1 if you have an indicator LED
#ifndef LED_ON_OFF_PIN
#define LED_ON_OFF_PIN 2
#endif

// data pin of the strip
#ifndef LED_PIN
  #if defined(ESP8266)
  #define LED_PIN 2
  #elif defined(ESP32)
  #define LED_PIN 14
  #else
  #error "Unsupported board class"
  #endif
#endif

// --------------
// Animation config
// --------------
#define ANIM_COUNT 10 // number of regular animations (indices 0..ANIM_COUNT-1); 0 is the start animation, 1..ANIM_COUNT-1 are cycled randomly
#define INTERVAL 30000 //change interval, msec
#define START_ANIMATION_MS 10000 // the start animation has a fixed duration, msec

#define MAXLEDS 1024 // maximum number of LEDs in the strip (buffers are allocated for the configured number)

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
// Both names get "-" + the last 3 bytes of the unit's MAC appended, so that several units don't clash:
// the setup access point is "Liana-A1B2C3", the mDNS / DHCP host name is "liana-a1b2c3" (liana-a1b2c3.local).
#ifndef WIFI_AP_SSID
#define WIFI_AP_SSID "Liana"
#endif
#define WIFI_CONNECTION_TIMEOUT 10000  //Timeout (in milliseconds) of waiting for WiFi connection
#ifndef MDNS_NAME
#define MDNS_NAME "liana"
#endif

// --------------
// Time and night mode config
// --------------
// POSIX timezone rule; applied without network access (this one is Europe/Warsaw)
#ifndef TIMEZONE_POSIX
#define TIMEZONE_POSIX "CET-1CEST,M3.5.0,M10.5.0/3"
#endif
// defaults for night mode; runtime values live in config.json
// (nightEnabled, nightStartHour, nightEndHour, nightAnim, nightBrightness)
#ifndef NIGHT_MODE_ENABLED
#define NIGHT_MODE_ENABLED 1
#endif
#define NIGHT_START_HOUR 21
#define NIGHT_END_HOUR 6
#define NIGHT_ANIMATION_INDEX 5 // stars
#define NIGHT_BRIGHTNESS 50

#endif
