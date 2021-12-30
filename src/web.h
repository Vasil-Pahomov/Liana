#ifndef web_h
#define web_h

#if defined(ESP8266)
extern ESP8266WebServer webServer;
#elif defined(ESP32)
extern WebServer webServer;
#else
#error "Unsupported board class"
#endif

void webSetup();

#endif