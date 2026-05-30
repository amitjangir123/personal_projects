#ifndef MOTION_DRIVER_H
#define MOTION_DRIVER_H

#include "config.h"
#ifdef __cplusplus
extern "C" {
#endif

// Convert Cartesian target to motor steps and execute a coordinated move.
// - currentMotorSteps: pointer to current absolute motor step counts
// - currentPos: current Cartesian position (mm)
// - targetPos: desired Cartesian target (mm). This will be modified by backlash
//   compensation before conversion to motor steps.
// - dir_state: pointer to DirectionState used/updated by backlash logic.
// - pulseStepFunc and setDirFunc: callbacks forwarded to moveLinearMotorSteps
void motion_driver_move_to(const MotorSteps* currentMotorSteps,
                           const Position* currentPos,
                           const Position* targetPos,
                           DirectionState* dir_state,
                           float feed_mm_per_min,
                           void (*delayFunc)(unsigned int),
                           void (*pulseStepFunc)(int),
                           void (*setDirFunc)(int, int));

#ifdef __cplusplus
}
#endif

#endif
