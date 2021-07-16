#include "switch.h"

/* SWITCH DEFINITIONS */
Switch switch1 = Switch(4U);
Switch switch2 = Switch(3U);
Switch switch3 = Switch(2U);

// MIDI PARAM
const uint8_t programChangeLine6M5 = 192;
//const uint8_t controlChangeLine6M5 = 176;
const uint8_t favPresetLine6M5 = 7;
uint8_t presetLine6M5 = favPresetLine6M5;

// Send MIDI message
void sendMidiProgramChange(int COMMAND, int DATA) {
    Serial.write(COMMAND);
    Serial.write(DATA);
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
        {
            // Press Switch 1
            if (presetLine6M5 > 0)
                presetLine6M5--;
            else
                presetLine6M5 = 23;  
            
            sendMidiProgramChange(programChangeLine6M5, presetLine6M5);
        }
        else if (strcmp(res->notifyValue, "PRE2") == 0)
        {
            // Press Switch 2
            presetLine6M5 = favPresetLine6M5;
          
            sendMidiProgramChange(programChangeLine6M5, presetLine6M5);
        }
        else if (strcmp(res->notifyValue, "PRE3") == 0)
        {
            // Press Switch 3
            if (presetLine6M5 < 23)
                presetLine6M5++;
            else
                presetLine6M5 = 0;
            
            sendMidiProgramChange(programChangeLine6M5, presetLine6M5);
        }
    //xTaskNotify(xTaskGetId("TASKSERIAL"), 4, res->notifyValue);
    }
    xMemFree(res);
    xTaskNotifyClear(id_);
}

void setup()
{
    Serial.begin(31250);
    //Serial.println(F("[MIDI Master Debug Stream]"));
    
    delay(3000); // Compensate for slow start Line6 M5
    sendMidiProgramChange(programChangeLine6M5, presetLine6M5);

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
