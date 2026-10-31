#include "hw_button.h"
#include "config.h"
#include "player.h"

#if defined(SUPPORT_BUTTON)

// Variables will change:
int buttonOnOffLastState = LOW; // the previous state from the input pin
int buttonOnOffCurrentState;    // the current reading from the input pin

#define DEBOUNCE_TIME 50 // the debounce time in millisecond, increase this time if it still chatters

unsigned long lastTime = 0;

static void _toggleOnOff()
{
    if (player.isOn())
    {
        Serial.println("Turn off");
        player.turnOff();
    }
    else
    {
        Serial.println("Turn on");
        player.turnOn();
    }
}

void buttonsLoop()
{
    if ((millis() - lastTime) > DEBOUNCE_TIME)
    {
        // read the state of the switch/button:
        buttonOnOffCurrentState = digitalRead(BUTTON_ON_OFF_PIN);

        if (buttonOnOffLastState == LOW && buttonOnOffCurrentState == HIGH)
        {
            Serial.println("The button ON/OFF is released");
            _toggleOnOff();
        }

        // save the the last state
        buttonOnOffLastState = buttonOnOffCurrentState;
        lastTime = millis();
    }
}

void buttonsSetUp()
{
    pinMode(BUTTON_ON_OFF_PIN, INPUT_PULLUP);
}

#else

void buttonsLoop() {}
void buttonsSetUp() {}

#endif
