#include "Arduino.h"
#include "config.h"
#include "gcode_executor.h"
#include "kinematics.h"
#include "motion.h"
#include "motion_driver.h"
#include "backlash.h"
#include <stdio.h>

static float g_last_feed_rate = 0.0f;

extern "C" void executeGcodeCommand(const GcodeCommand* cmd,
                         Position* currentPos,
                         MotorSteps* currentSteps,
                         DirectionState* dir_state,
                         int* absolute_mode,
                         void (*pulseStepFunc)(int),
                         void (*setDirFunc)(int, int),
                         void (*delayFunc)(unsigned int)) {
  if (!cmd || !currentPos || !currentSteps || !dir_state || !absolute_mode) return;

  if (cmd->command == 'G') {
    if (cmd->code == 90) {
      *absolute_mode = 1;
      (void)cmd;
    } else if (cmd->code == 92) {
      if (cmd->hasX) currentPos->x = cmd->x;
      if (cmd->hasY) currentPos->y = cmd->y;
      if (cmd->hasZ) currentPos->z = cmd->z;
      *currentSteps = cartesianToMotors(currentPos->x, currentPos->y, currentPos->z);
      return;
    }

    if (cmd->code == 0 || cmd->code == 1) {
      Position target = *currentPos;

      if (*absolute_mode) {
        if (cmd->hasX) target.x = cmd->x;
        if (cmd->hasY) target.y = cmd->y;
        if (cmd->hasZ) target.z = cmd->z;
      } else {
        if (cmd->hasX) target.x += cmd->x;
        if (cmd->hasY) target.y += cmd->y;
        if (cmd->hasZ) target.z += cmd->z;
      }

      const float MIN_FEEDRATE = 1.0f;
      float feed = 0.0f;
      if (cmd->hasF && cmd->f > 0.0f) {
        feed = cmd->f;
        if (feed > MAX_FEEDRATE) feed = MAX_FEEDRATE;
        if (feed < MIN_FEEDRATE) feed = MIN_FEEDRATE;
        g_last_feed_rate = feed;
        //Serial1.println("[STM32]: Using feedrate from command");
        //Serial1.println(feed);
      } else if (g_last_feed_rate > 0.0f) {
        feed = g_last_feed_rate;
        //Serial1.println("[STM32]: No feedrate specified; reusing last_feed_rate");
        //Serial1.println(feed);
      } else {
        feed = MAX_FEEDRATE;
        //Serial1.println("[STM32]: No feedrate specified and no previous feed; using MAX_FEEDRATE");
        //Serial1.println(feed);
      }
      motion_driver_move_to(currentSteps, currentPos, &target, dir_state, feed, delayFunc, pulseStepFunc, setDirFunc);

      *currentPos = target;
      *currentSteps = cartesianToMotors(currentPos->x, currentPos->y, currentPos->z);
      return;
    }
  }

  if (cmd->command == 'M') {
    if (cmd->code == 3) {
      return;
    }
  }
}
