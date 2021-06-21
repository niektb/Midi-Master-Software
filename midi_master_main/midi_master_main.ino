#include "switch.h"

Switch switch1 = Switch(4U);
Switch switch2 = Switch(3U);
Switch switch3 = Switch(2U);

/* TASKS */
void taskSW1(xTaskId id_)
{
  // Call the task function of switch1.
  switch1.TaskFunction(id_);
}

void taskSW2(xTaskId id_)
{
  // Call the task function of switch2.
  switch2.TaskFunction(id_);
}

void taskSW3(xTaskId id_)
{
  // Call the task function of switch3.
  switch3.TaskFunction(id_);
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

  xHeliOSSetup();
  
  xTaskId id = xTaskAdd("TASKSW1", &taskSW1);
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
