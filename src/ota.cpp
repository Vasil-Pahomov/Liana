
#include "ota.h"
#include "anim.h"

#ifdef SUPPORT_OTA
    #include <ArduinoOTA.h>
#endif

int otaProgress = 0;

void otaSetUp(){
#ifdef SUPPORT_OTA
    Serial.println("OTA is setuping");
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setPort(8266);
ArduinoOTA.onStart([]() {
    otaProgress = 0;
    animInd = ANIMATION_OTA_INDEX;
    setAnimPal();
    Serial.println("OTA-Start");  //  "Начало OTA-апдейта"
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA-End"); 
    otaProgress = 100; //  "Завершение OTA-апдейта"
    anim.run();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    otaProgress = (progress / (total / 100));
    Serial.printf("Progress: %u%%\r", otaProgress);
    anim.run();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA-Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    //  "Ошибка при аутентификации"
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    //  "Ошибка при начале OTA-апдейта"
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    //  "Ошибка при подключении"
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    //  "Ошибка при получении данных"
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    //  "Ошибка при завершении OTA-апдейта"
  });
  Serial.println("OTA setuped");
#endif
}

void otaBegin(){
#ifdef SUPPORT_OTA
  Serial.println("OTA is beginning");
  otaProgress = 0;
  animInd = ANIMATION_OTA_INDEX;
  setAnimPal();
  ArduinoOTA.begin();
#endif
}

void otaLoop(){
#ifdef SUPPORT_OTA
        ArduinoOTA.handle();
#endif
}