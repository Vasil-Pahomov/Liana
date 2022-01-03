#ifndef OTA_H_INCLUDED
#define OTA_H_INCLUDED

#define SUPPORT_OTA
#define OTA_PASSWORD (const char *)"123OtA!_456"

void otaSetUp();

void otaBegin();

void otaLoop();

extern int otaProgress;

#endif