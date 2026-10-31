#ifndef FS_COMPAT_H
#define FS_COMPAT_H

// The flash file system used for pages and the config file, same name on both platforms.
#include <FS.h>
#if defined(ESP8266)
    #define LIANA_FS SPIFFS
#elif defined(ESP32)
    #include <SPIFFS.h>
    #define LIANA_FS SPIFFS
#else
    #error "Unsupported board class"
#endif

#endif
