#include "switch.h"

Switch::Switch(const uint8_t pin) :
    PIN(pin),
    switchState(HIGH),
    prevSwitchState(HIGH),
    lastDebounceTime(0U),
    firstTapTime(0U),
    firstReleaseTime(0U),
    WaitForRelease(false),
    prevButtonState(HIGH)
{
    pinMode(this->PIN, INPUT);
}

Switch::~Switch()
{
}

void Switch::TaskFunction(xTaskId id_, const char pre[4], const char hold[4], const char rel[4])
{
    /* DEBOUNCE PARAMETERS */
    const uint8_t debounce_delay = 20;
    
    int buttonState = digitalRead(this->PIN);
    
    // reset debouncing timer if the switch changed, due to noise or pressing:
    if (buttonState != this->prevButtonState)
    {
      this->lastDebounceTime = millis();
    }
    
    // if the button state is stable for at least [debounce_delay], fire body of the statement
    if ((millis() - this->lastDebounceTime) > debounce_delay)
    {
        // buttonState and lastButtonState represent the 'unstable' input that gets updated continuously.
        // these are used for debouncing.
        // switchState is the stable input that can be used for reading button presses.
        if (buttonState != this->switchState)
        {
            this->switchState = buttonState;
        }
        
        if ((this->switchState == LOW) && (this->prevSwitchState == HIGH)) // SWITCH PRESS
        {
            this->firstTapTime = millis();
            this->WaitForRelease = true;
            
            // PERFORM PRESS ACTION
            xTaskNotify(xTaskGetId("TASKMAN"), 4, pre);
            
            this->prevSwitchState = this->switchState;
        }
        else if (this->WaitForRelease && ((millis() - this->firstTapTime) >= 1000)) // SWITCH HOLD
        {
            this->WaitForRelease = false;
            
            // PERFORM HOLD ACTION
            xTaskNotify(xTaskGetId("TASKMAN"), 4, hold);
        }
        else if (this->switchState == HIGH && this->prevSwitchState == LOW) // SWITCH RELEASE
        {
            if (this->WaitForRelease)
            {
              this->firstReleaseTime = millis();
            
              // PERFORM RELEASE ACTION
              xTaskNotify(xTaskGetId("TASKMAN"), 4, rel);
            
              this->WaitForRelease = false;
            }
            this->prevSwitchState = this->switchState;
        }
    }
    this->prevButtonState = buttonState;
}