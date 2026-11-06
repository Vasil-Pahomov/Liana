
#include "ota.h"
#include "anim.h"
#include "player.h"
#include "liana_wifi.h"

#ifdef SUPPORT_OTA
    #include <ArduinoOTA.h>
#endif

int otaProgress = 0;

static void otaShowProgressBar() {
    otaProgress = 0;
    player.select(ANIMATION_OTA_INDEX, player.currentPalette());
}

void otaSetUp(){
#ifdef SUPPORT_OTA
    Serial.println("OTA is setuping");
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.setPort(8266);
    //ArduinoOTA.begin() restarts mDNS under this name (default "esp8266-xxxxxx"/"esp32-xxxx"), keep ours
    ArduinoOTA.setHostname(deviceHostName().c_str());
    ArduinoOTA.onStart([]() {
        otaShowProgressBar();
        Serial.println("OTA-Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA-End");
        otaProgress = 100;
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
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    Serial.println("OTA setuped");
#endif
}

//arms the OTA listener (from the web page); the strip shows the progress bar until the update arrives
void otaBegin(){
#ifdef SUPPORT_OTA
    Serial.println("OTA is beginning");
    otaShowProgressBar();
    ArduinoOTA.begin();
#endif
}

void otaLoop(){
#ifdef SUPPORT_OTA
    ArduinoOTA.handle();
#endif
}
