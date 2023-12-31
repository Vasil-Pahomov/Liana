#include "hw_button.h"
#include "config.h"
#include "anim.h"

// Variables will change:
int buttonOnOffLastState = LOW; // the previous state from the input pin
int buttonOnOffCurrentState;    // the current reading from the input pin

int buttonOnOffState = 0;

#define DEBOUNCE_TIME 50 // the debounce time in millisecond, increase this time if it still chatters

unsigned long lastTime = 0;

void buttonsLoop()
{
    #if defined(SUPPORT_BUTTON)
    if ((millis() - lastTime) > DEBOUNCE_TIME)
    {
        // read the state of the switch/button:
        buttonOnOffCurrentState = digitalRead(BUTTON_ON_OFF_PIN);

        // if (buttonOnOffLastState == HIGH && buttonOnOffCurrentState == LOW)
        //{     Serial.println("The button is pressed");} else
        if (buttonOnOffLastState == LOW && buttonOnOffCurrentState == HIGH)
        {
            Serial.println("The button ON/OFF is released");
            _toggleOnOff();
        }

        // save the the last state
        buttonOnOffLastState = buttonOnOffCurrentState;
        lastTime = millis();
    }
    #endif
}

void _toggleOnOff()
{
    if (animInd == ANIMATION_OFF_INDEX)
    { // OFF? -> on
        Serial.println("Turn on");
        animInd = ANIMATION_START_INDEX;
    }
    else
    { // ON? -> off
        Serial.println("Turn off");
        animInd = ANIMATION_OFF_INDEX;
    }
    setAnimPal();
}

void buttonsSetUp()
{
    #if defined(SUPPORT_BUTTON)
    pinMode(BUTTON_ON_OFF_PIN, INPUT_PULLUP);
    #endif
}