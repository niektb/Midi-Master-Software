#include <HeliOS_Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>

/* PIN DEFINITIONS */
const uint8_t pin_switch_1  = ;
const uint8_t pin_switch_2  = ;
const uint8_t pin_switch_3  = ;
const uint8_t pin_midi_tx   = ;

/* DEBOUNCE PARAMETERS */
const uint8_t debounce_delay = 20;

// SWITCH 1
unsigned long last_debounce_time_1 = 0;
bool s1state;
bool last_s1state = HIGH;
bool last_button1 = HIGH;

// SWITCH 3
unsigned long last_debounce_time_2 = 0;
bool s2state;
bool last_s2state = HIGH;
bool last_button2 = HIGH;

// SWITCH 3
unsigned long last_debounce_time_3 = 0;
bool s3state;
bool last_s3state = HIGH;
bool last_button3 = HIGH;

/* Line6 M5 Parameters */
// current number?
// favorite?
// preset 1, 2 and 3?

/* TASKS */
void taskSW1(xTaskId id_)
{
  int button = digitalRead(pin_switch_1);
  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (button != last_button1)
    last_debounce_time = millis();

  // If the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - last_debounce_time_1) > debounce_delay) {

    // Button and last_button represent the 'unstable' input that gets updated continuously.
    // These are used for debouncing.
    // s1state is the stable input that can be used for reading button presses.
    if (button != s1state)
      s1state = button;

    if (s1state == LOW && last_s1state == HIGH) // SWITCH PRESS
    {
      first_tap_time = millis();
      wait_for_release = true;
      last_s1state = s1state;
    }
    else if (wait_for_release && ((millis() - first_tap_time) >= 1000)) // SWITCH HOLD
    {
      wait_for_release = false;
      // PERFORM HOLD ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"HOLD" );
    }
    else if (s1state == HIGH && last_s1state == LOW) // SWITCH RELEASE
    {
      if (wait_for_release)
      {
        first_release_time = millis();

        // PERFORM RELEASE ACTION
        xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"RELA" );

        wait_for_release = false;
      }
      last_s1state = s1state;
    }
  }
  last_button1 = button;
}

void taskSerial(xTaskId id_)
{
  xTaskGetNotifResult res = xTaskGetNotif(id_);
  if (res)
    Serial.println(res->notifyValue);
  xMemFree(res);
  xTaskNotifyClear(id_);
}

void taskMidiMan(xTaskId id_)
{
  // General Purpose Task to keep track of the MIDI Controller State
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("[VDD Debug Stream]"));

  // PIN SETUP
  pinMode(pin_switch_1, INPUT);
  pinMode(pin_switch_2, INPUT);
  pinMode(pin_switch_3, INPUT);
  pinMode(pin_delay_time, OUTPUT);


  xTaskId id = 0;
  xHeliOSSetup();

}


void loop()
{
  xHeliOSLoop();
}
