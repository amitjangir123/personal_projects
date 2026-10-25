#include <Arduino.h>
#include "common_functions.h"

// Current can be between 0 and 360, target can be between 0 and 360. Error can be between -360 and +360
float headingError(float target, float current)
{
    float error = target - current;
    if (error > 180.0f)
    {
        error -= 360.0f;
    }
    else if (error < -180.0f)
    {
        error += 360.0f;
    }
    return error;
}

float normalizeHeading(float heading)
{
    if(heading >= 360.0f)
    {
        heading -= 360.0f;
    }
    else if(heading < 0.0f)
    {
        heading += 360.0f;
    }
    return heading;
}

void myDelay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void sendStm32Cmd(int32_t leftSpeed, uint32_t leftSteps, int32_t rightSpeed, uint32_t rightSteps)
{
    Serial2.println("L:" + String(leftSpeed) + " LS:" + String(leftSteps) + " R:" + String(rightSpeed) + " RS:" + String(rightSteps));
}