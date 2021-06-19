#include <HeliOS_Arduino.h>

/* PIN DEFINITIONS */
const uint8_t pin_sw1  = 4;
const uint8_t pin_sw2  = 3;
const uint8_t pin_sw3  = 2;

/* DEBOUNCE PARAMETERS */
const uint8_t debounce_delay = 20;

// SWITCH 1
unsigned long ldt1 = 0; // last debounce time
bool s1state;
bool last_s1state = HIGH;
bool last_button1 = HIGH;

unsigned long ftt1; // first tap time
unsigned long frt1; // first release time
bool wfr1 = false;  // wait for release

// SWITCH 3
unsigned long ldt2 = 0;
bool s2state;
bool last_s2state = HIGH;
bool last_button2 = HIGH;

unsigned long ftt2; // first tap time
unsigned long frt2; // first release time
bool wfr2 = false;  // wait for release

// SWITCH 3
unsigned long ldt3 = 0;
bool s3state;
bool last_s3state = HIGH;
bool last_button3 = HIGH;

unsigned long ftt3; // first tap time
unsigned long frt3; // first release time
bool wfr3 = false;  // wait for release

/* TASKS */
void taskSW1(xTaskId id_)
{
  int button = digitalRead(pin_sw1);
  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (button != last_button1)
    ldt1 = millis();

  // If the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - ldt1) > debounce_delay) {

    // Button and last_button represent the 'unstable' input that gets updated continuously.
    // These are used for debouncing.
    // s1state is the stable input that can be used for reading button presses.
    if (button != s1state)
      s1state = button;

    if (s1state == LOW && last_s1state == HIGH) // SWITCH PRESS
    {
      ftt1 = millis();
      wfr1 = true;

      // PERFORM PRESS ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"PRE1" );
      
      last_s1state = s1state;
    }
    else if (wfr1 && ((millis() - ftt1) >= 1000)) // SWITCH HOLD
    {
      wfr1 = false;
      // PERFORM HOLD ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"HOL1" );
    }
    else if (s1state == HIGH && last_s1state == LOW) // SWITCH RELEASE
    {
      if (wfr1)
      {
        frt1 = millis();

        // PERFORM RELEASE ACTION
        xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"REL1" );

        wfr1 = false;
      }
      last_s1state = s1state;
    }
  }
  last_button1 = button;
}

void taskSW2(xTaskId id_)
{
  int button = digitalRead(pin_sw2);
  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (button != last_button2)
    ldt2 = millis();

  // If the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - ldt2) > debounce_delay) {

    // Button and last_button represent the 'unstable' input that gets updated continuously.
    // These are used for debouncing.
    // s1state is the stable input that can be used for reading button presses.
    if (button != s2state)
      s2state = button;

    if (s2state == LOW && last_s2state == HIGH) // SWITCH PRESS
    {
      ftt2 = millis();
      wfr2 = true;

      // PERFORM PRESS ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"PRE2");
      
      last_s2state = s2state;
    }
    else if (wfr2 && ((millis() - ftt2) >= 1000)) // SWITCH HOLD
    {
      wfr2 = false;
      // PERFORM HOLD ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"HOL2");
    }
    else if (s2state == HIGH && last_s2state == LOW) // SWITCH RELEASE
    {
      if (wfr2)
      {
        frt2 = millis();

        // PERFORM RELEASE ACTION
        xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"REL2");

        wfr2 = false;
      }
      last_s2state = s1state;
    }
  }
  last_button2 = button;
}

void taskSW3(xTaskId id_)
{
  int button = digitalRead(pin_sw3);
  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (button != last_button3)
    ldt3 = millis();

  // If the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - ldt3) > debounce_delay) {

    // Button and last_button represent the 'unstable' input that gets updated continuously.
    // These are used for debouncing.
    // s3state is the stable input that can be used for reading button presses.
    if (button != s3state)
      s3state = button;

    if (s3state == LOW && last_s3state == HIGH) // SWITCH PRESS
    {
      ftt3 = millis();
      wfr3 = true;

      // PERFORM PRESS ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"PRE3");
      
      last_s3state = s3state;
    }
    else if (wfr3 && ((millis() - ftt3) >= 1000)) // SWITCH HOLD
    {
      wfr3 = false;
      // PERFORM HOLD ACTION
      xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"HOL3");
    }
    else if (s3state == HIGH && last_s3state == LOW) // SWITCH RELEASE
    {
      if (wfr3)
      {
        frt3 = millis();

        // PERFORM RELEASE ACTION
        xTaskNotify(xTaskGetId("TASKSERIAL"), 4, (char *)"REL3");

        wfr3 = false;
      }
      last_s3state = s1state;
    }
  }
  last_button3 = button;
}

void taskSerial(xTaskId id_)
{
  xTaskGetNotifResult res = xTaskGetNotif(id_);
  if (res)
    Serial.println(res->notifyValue);
  xMemFree(res);
  xTaskNotifyClear(id_);
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("[MIDI Master Debug Stream]"));

  // PIN SETUP
  pinMode(pin_sw1, INPUT);
  pinMode(pin_sw2, INPUT);
  pinMode(pin_sw3, INPUT);

  xTaskId id = 0;
  xHeliOSSetup();
  
  id = xTaskAdd("TASKSW1", &taskSW1);
  xTaskStart(id);

  id = xTaskAdd("TASKSW2", &taskSW2);
  xTaskStart(id);

  id = xTaskAdd("TASKSW3", &taskSW3);
  xTaskStart(id);

  id = xTaskAdd("TASKSERIAL", &taskSerial);
  xTaskWait(id);
}

void loop()
{
  xHeliOSLoop();
}
