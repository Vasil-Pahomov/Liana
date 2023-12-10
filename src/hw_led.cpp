#include "hw_led.h"
#include "config.h"

void ledSetUp()
{
    #if defined(SUPPORT_LED_ON_OFF)
    pinMode(LED_ON_OFF_PIN, OUTPUT);
    #endif
}

void ledTurnOn()
{
    #if defined(SUPPORT_LED_ON_OFF)
    digitalWrite(LED_ON_OFF_PIN, HIGH);
    #endif
}
void ledTurnOff()
{
    #if defined(SUPPORT_LED_ON_OFF)
    digitalWrite(LED_ON_OFF_PIN, LOW);
    #endif
}