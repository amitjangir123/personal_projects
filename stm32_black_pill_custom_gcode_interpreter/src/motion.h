#ifndef MOTION_H
#define MOTION_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// moveLinearMotorSteps
void moveLinearMotorSteps(const MotorSteps* current, const MotorSteps* target,
                          void (*pulseStepFunc)(int),
                          void (*setDirFunc)(int, int));

// Timed variant: path length in mm and feed in mm/min
void moveLinearMotorSteps_timed(const MotorSteps* current, const MotorSteps* target,
                               void (*pulseStepFunc)(int),
                               void (*setDirFunc)(int, int),
                               void (*delayFunc)(unsigned int),
                               float feed_mm_per_min,
                               float path_length_mm);

// Mask variant: pulses multiple pins simultaneously
void moveLinearMotorSteps_mask(const MotorSteps* current, const MotorSteps* target,
                               void (*pulseMaskFunc)(unsigned int),
                               void (*setDirFunc)(int, int));

#ifdef __cplusplus
}
#endif

#endif
