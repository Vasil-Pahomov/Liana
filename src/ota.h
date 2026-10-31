#ifndef OTA_H_INCLUDED
#define OTA_H_INCLUDED

#define SUPPORT_OTA

// Password of ArduinoOTA. Override it with a build flag in platformio_local.ini:
//   build_flags = -DOTA_PASSWORD=\"your-password\"
#ifndef OTA_PASSWORD
#define OTA_PASSWORD "123OtA!_456"
#endif

void otaSetUp();

void otaBegin();

void otaLoop();

extern int otaProgress;

#endif
