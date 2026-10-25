#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <Arduino.h>

float headingError(float target, float current);
float normalizeHeading(float heading);
void  myDelay(uint32_t);
void sendStm32Cmd(int32_t leftSpeed, uint32_t leftSteps, int32_t rightSpeed, uint32_t rightSteps);

#endif // COMMON_FUNCTIONS_H