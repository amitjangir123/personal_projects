#ifndef GCODE_EXECUTOR_H
#define GCODE_EXECUTOR_H

#include "config.h"
#include "gcode_parser.h"
#ifdef __cplusplus
extern "C" {
#endif

// Execute a parsed G-code command.
// - cmd: parsed G-code command
// - currentPos: current Cartesian position (mm) — will be updated after move
// - currentSteps: current motor absolute step counts — will be updated after move
// - dir_state: direction state for backlash handling
// - absolute_mode: pointer to int holding 1=absolute, 0=relative; modified by G90/G91
// - pulseStepFunc / setDirFunc: hardware callbacks forwarded to motion driver
void executeGcodeCommand(const GcodeCommand* cmd,
                         Position* currentPos,
                         MotorSteps* currentSteps,
                         DirectionState* dir_state,
                         int* absolute_mode,
                         void (*pulseStepFunc)(int),
                         void (*setDirFunc)(int, int),
                         void (*delayFunc)(unsigned int));

#ifdef __cplusplus
}
#endif

#endif
