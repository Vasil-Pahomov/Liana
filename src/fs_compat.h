#ifndef FS_COMPAT_H
#define FS_COMPAT_H

// The flash file system that holds the pages and the config file: LittleFS on both platforms.
// The image is built by "pio run -t buildfs" (board_build.filesystem = littlefs in platformio.ini).
#include <FS.h>
#include <LittleFS.h>
#define LIANA_FS LittleFS

// mounts the file system, formatting it when it is empty or damaged
inline bool lianaFsBegin() {
#if defined(ESP8266)
    return LittleFS.begin(); // formats on mount failure by default (LittleFSConfig autoFormat)
#elif defined(ESP32)
    return LittleFS.begin(true);
#else
    #error "Unsupported board class"
#endif
}

#endif
