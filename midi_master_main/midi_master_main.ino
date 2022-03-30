#include <HeliOS.h>

/* PIN DEFINITIONS */
const uint8_t pin_sw1  = 4;
const uint8_t pin_sw2  = 3;
const uint8_t pin_sw3  = 2;

/* DEBOUNCE PARAMETERS */
const uint8_t debounce_delay = 20;

// SWITCH
// Define struct to pass to task
typedef struct footsw {
    int port;
    bool sstate;
    bool last_sstate;
    bool last_button;

    unsigned long ldt; // last debounce time
    unsigned long ftt; // first tap time
    unsigned long frt; // first release time
    bool wfr;  // wait for release
    char sw_press[4];
    char sw_hold[4];
    char sw_release[4];
} footsw;

// MIDI PARAM
const uint8_t fav_preset = 7; // preset 8
uint8_t current_preset = fav_preset;
const int PC = 192;
//const int CC = 176;

// Send MIDI message
void MIDIPC(int COMMAND, int DATA) {
  Serial.write(COMMAND);
  Serial.write(DATA);
}

/* TASKS */
void taskSW_main(xTask task_, xTaskParm parm_)
{
  footsw sw = DEREF_TASKPARM(footsw, parm_);

  int button = digitalRead(sw.port);
  // reset debouncing timer if the switch changed, due to noise or pressing:
  if (button != sw.last_button)
    sw.ldt = millis();

  // If the button state is stable for at least [debounce_delay], fire body of the statement
  if ((millis() - sw.ldt) > debounce_delay) {

    // Button and last_button represent the 'unstable' input that gets updated continuously.
    // These are used for debouncing.
    // sstate is the stable input that can be used for reading button presses.
    if (button != sw.sstate)
      sw.sstate = button;

    if (sw.sstate == LOW && sw.last_sstate == HIGH) // SWITCH PRESS
    {
      sw.ftt = millis();
      sw.wfr = true;

      // PERFORM PRESS ACTION
      xTaskNotifyGive(xTaskGetHandleByName("TASKMAN"), 4, sw.sw_press);
      
      sw.last_sstate = sw.sstate;
    }
    else if (sw.wfr && ((millis() - sw.ftt) >= 1000)) // SWITCH HOLD
    {
      sw.wfr = false;
      // PERFORM HOLD ACTION
      xTaskNotifyGive(xTaskGetHandleByName("TASKMAN"), 4, sw.sw_hold);
    }
    else if (sw.sstate == HIGH && sw.last_sstate == LOW) // SWITCH RELEASE
    {
      if (sw.wfr)
      {
        sw.frt = millis();

        // PERFORM RELEASE ACTION
        xTaskNotifyGive(xTaskGetHandleByName("TASKMAN"), 4, sw.sw_release);

        sw.wfr = false;
      }
      sw.last_sstate = sw.sstate;
    }
  }
  sw.last_button = button;

  DEREF_TASKPARM(footsw, parm_) = sw;
  return;
}

void taskSerial_main(xTask task_, xTaskParm parm_)
{
  xTaskNotification res = xTaskNotifyTake(task_);
  if (res)
    Serial.println(res->notificationValue);
  xMemFree(res);
}

void taskMan_main(xTask task_, xTaskParm parm_)
{
  xTaskNotification res = xTaskNotifyTake(task_);
  if (res)
  {
    // Perform task if we got a notification
    if (strcmp(res->notificationValue, "PRE1") == 0)
    {
      // Press Switch 1
      if (current_preset > 0)
        current_preset--;
      else
        current_preset = 23;  
      
      MIDIPC(PC, current_preset);
    }
    else if (strcmp(res->notificationValue, "PRE2") == 0)
    {
      // Press Switch 2
        current_preset = fav_preset;
      
      MIDIPC(PC, current_preset);
    }
    else if (strcmp(res->notificationValue, "PRE3") == 0)
    {
      // Press Switch 3
      if (current_preset < 23)
        current_preset++;
      else
        current_preset = 0;
        
      MIDIPC(PC, current_preset);
    }
    //xTaskNotifyGive(xTaskGetHandleByName("TASKSERIAL")), 4, res->notificationValue);
  }
  xMemFree(res);
}

void setup()
{
  Serial.begin(31250);
  //Serial.println(F("[MIDI Master Debug Stream]"));

  // PIN SETUP
  pinMode(pin_sw1, INPUT);
  pinMode(pin_sw2, INPUT);
  pinMode(pin_sw3, INPUT);

  delay(3000); // Compensate for slow start Line6 M5
  MIDIPC(PC, current_preset);

    // initialize struct for switch 1
    footsw SW1 = {
        pin_sw1,    //port
        HIGH,       //sstate
        HIGH,       //last_sstate
        HIGH,       //last_button
        0,          //ldt
        0,          //fft
        0,          //frt
        false,      //wfr
        (char *)"PRE1", //press
        (char *)"HOL1", //hold
        (char *)"REL1"  //release
    };
    xTask taskSW1 = xTaskCreate("TASKSW1", taskSW_main, &SW1);

    // initialize struct for switch 2
    footsw SW2 = {
        pin_sw2,    //port
        HIGH,       //sstate
        HIGH,       //last_sstate
        HIGH,       //last_button
        0,          //ldt
        0,          //fft
        0,          //frt
        false,      //wfr
        (char *)"PRE2", //press
        (char *)"HOL2", //hold
        (char *)"REL2"  //release
    };
    xTask taskSW2 = xTaskCreate("TASKSW2", taskSW_main, &SW2);

    // initialize struct for switch 1
    footsw SW3 = {
        pin_sw3,    //port
        HIGH,       //sstate
        HIGH,       //last_sstate
        HIGH,       //last_button
        0,          //ldt
        0,          //fft
        0,          //frt
        false,      //wfr
        (char *)"PRE3", //press
        (char *)"HOL3", //hold
        (char *)"REL3"  //release
    };
    xTask taskSW3 = xTaskCreate("TASKSW1", taskSW_main, &SW3);

    //xTask taskSerial = xTaskCreate("TASKSERIAL", taskSerial_main, NULL);
    
    xTask taskMan = xTaskCreate("TASKMAN", taskSW_main, NULL);

    // Check if all tasks are created correctly
    if (taskSW1 && taskSW2 && taskSW3 && taskMan /*&& taskSerial*/) {
        xTaskResume(taskSW1);
        xTaskResume(taskSW2);
        xTaskResume(taskSW3);
        xTaskWait(taskMan);
        //xTaskWait(taskSerial);

        xTaskStartScheduler();

        xTaskDelete(taskSW1);
        xTaskDelete(taskSW2);
        xTaskDelete(taskSW3);
        xTaskDelete(taskMan);
        //xTaskDelete(taskSerial);
    }
    xSystemHalt();
}

void loop()
{
  // Should remain empty
}
