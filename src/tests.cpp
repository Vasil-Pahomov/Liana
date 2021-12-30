#include <config.h>

void test_printEnabledAnims() {
    Serial.print(currentConfig.getEnabledAnimsCount());Serial.print(':');
    for (int i=0;i<16;i++) { 
        if (currentConfig.isAnimEnabled(i)) { Serial.print(i); } else { Serial.print('-');}
        Serial.print(' ');
    }
    Serial.println();
}

void test_printEnabledPals() {
    Serial.print(currentConfig.getEnabledPalsCount());Serial.print(':');
    for (int i=0;i<16;i++) { 
        if (currentConfig.isPalEnabled(i)) { Serial.print(i); } else { Serial.print('-');}
        Serial.print(' ');
    }
    Serial.println();
}

void test_configEnableAnims() {
    Serial.print(F("Init,           "));test_printEnabledAnims();

    currentConfig.setAnimEnabled(0,false);
    Serial.print(F("Set 0 disabled, "));test_printEnabledAnims();

    currentConfig.setAnimEnabled(5,false);
    Serial.print(F("Set 5 disabled, "));test_printEnabledAnims();

    currentConfig.setAnimEnabled(0,true);
    Serial.print(F("Set 0 enabled,  "));test_printEnabledAnims();

    currentConfig.setAnimEnabled(5,true);
    Serial.print(F("Set 5 enabled,  "));test_printEnabledAnims();
}

void test_configEnablePals() {
    Serial.print(F("Init,           "));test_printEnabledPals();

    currentConfig.setPalEnabled(0,false);
    Serial.print(F("Set 0 disabled, "));test_printEnabledPals();

    currentConfig.setPalEnabled(5,false);
    Serial.print(F("Set 5 disabled, "));test_printEnabledPals();

    currentConfig.setPalEnabled(0,true);
    Serial.print(F("Set 0 enabled,  "));test_printEnabledPals();

    currentConfig.setPalEnabled(5,true);
    Serial.print(F("Set 5 enabled,  "));test_printEnabledPals();
}

void tests_runAll() {
    Serial.println(F("test_configEnableAnims"));
    test_configEnableAnims();
    Serial.println(F("test_configEnablePals"));
    test_configEnablePals();
}
