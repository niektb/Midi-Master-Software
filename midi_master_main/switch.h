#pragma once

#include <HeliOS_Arduino.h>

class Switch
{
public:
    ///
    /// Constructor.
    ///
    /// @param pin  Pin number of the switch.
    ///
    Switch(const uint8_t pin);
    
    ///
    /// Destructor.
    ///
    ~Switch();
    
    ///
    /// Function to execute in the task.
    ///
    /// @param id_  ID of the calling task.
    ///
    void TaskFunction(xTaskId id_, const char pre[4], const char hold[4], const char rela[4]);

private:
    /// Pin number of the switch.
    const uint8_t PIN;
    
    /// Stable input that can be used for reading button.
    int switchState;
    /// Previous stable input state.
    int prevSwitchState = HIGH;
    
    /// Last debounce time.
    uint32_t lastDebounceTime;
    /// First tap time
    uint32_t firstTapTime;
    /// First release time
    uint32_t firstReleaseTime;
    
    /// Wait for release
    bool WaitForRelease;
    
    /// Raw previous button state used for debouncing.
    int prevButtonState = HIGH;
};