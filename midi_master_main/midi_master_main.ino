#include "switch.h"

/* SWITCH DEFINITIONS */
Switch switch1 = Switch(4U);
Switch switch2 = Switch(3U);
Switch switch3 = Switch(2U);

// MIDI PARAM
const uint8_t controlChangeBossRC500 = 176;

// Send MIDI message
void sendMidiProgramChange(int COMMAND, int DATA) {
    Serial.write(COMMAND);
    Serial.write(DATA);
}

void sendMidiControlChange(int COMMAND, int DATA1, int DATA2) {
    Serial.write(COMMAND);
    Serial.write(DATA1);
    Serial.write(DATA2);
}

/* TASKS */
void taskSW1(xTaskId id_)
{
    // Call the task function of switch1.
    switch1.TaskFunction(id_, (char *)"PRE1", (char *)"HOL1", (char *)"REL1");
}

void taskSW2(xTaskId id_)
{
    // Call the task function of switch2.
    switch2.TaskFunction(id_, (char *)"PRE2", (char *)"HOL2", (char *)"REL2");
}

void taskSW3(xTaskId id_)
{
    // Call the task function of switch3.
    switch3.TaskFunction(id_, (char *)"PRE3", (char *)"HOL3", (char *)"REL3");
}

void taskSerial(xTaskId id_)
{
    xTaskGetNotifResult res = xTaskGetNotif(id_);
    if (res)
        Serial.println(res->notifyValue);
    xMemFree(res);
    xTaskNotifyClear(id_);
}

void taskMan(xTaskId id_)
{
    xTaskGetNotifResult res = xTaskGetNotif(id_);
    if (res)
    {
        // Perform task if we got a notification
        if (strcmp(res->notifyValue, "PRE1") == 0)
            sendMidiControlChange(controlChangeBossRC500, 16, 127);
        else if (strcmp(res->notifyValue, "PRE2") == 0)
            sendMidiControlChange(controlChangeBossRC500, 17, 127);
        else if (strcmp(res->notifyValue, "PRE3") == 0)
            sendMidiControlChange(controlChangeBossRC500, 18, 127);
    }
    xMemFree(res);
    xTaskNotifyClear(id_);
}

void setup()
{
    Serial.begin(31250);
    //Serial.println(F("[MIDI Master Debug Stream]"));
    
    delay(3000); // Compensate for slow start Controlled Device

    xHeliOSSetup();
    
    xTaskId id = xTaskAdd("TASKSW1", &taskSW1);
    xTaskStart(id);
    
    id = xTaskAdd("TASKSW2", &taskSW2);
    xTaskStart(id);
    
    id = xTaskAdd("TASKSW3", &taskSW3);
    xTaskStart(id);
    
    id = xTaskAdd("TASKMAN", &taskMan);
    xTaskWait(id);
}

void loop()
{
    xHeliOSLoop();
}
