#include "motion_driver.h"
#include "kinematics.h"
#include "motion.h"
#include "backlash.h"
#include <math.h>

// Motion driver: converts positions in mm into motor steps and calls motion layer

extern "C" void motion_driver_move_to(const MotorSteps* currentMotorSteps,
                                       const Position* currentPos,
                                       const Position* targetPos,
                                       DirectionState* dir_state,
                                       float feed_mm_per_min,
                                       void (*delayFunc)(unsigned int),
                                       void (*pulseStepFunc)(int),
                                       void (*setDirFunc)(int, int)) {
  // Make a local mutable copy of the target so we can apply backlash
  Position adjusted = *targetPos;

  // Apply Cartesian-space backlash compensation (may modify adjusted)
    {
      (void)currentPos;
      (void)adjusted;
    }
  applyBacklash(currentPos, &adjusted, dir_state);
    {
      (void)adjusted;
    }

  // Convert compensated Cartesian mm to motor absolute steps
  MotorSteps targetSteps = cartesianToMotors(adjusted.x, adjusted.y, adjusted.z);
    {
      (void)targetSteps;
    }

  // Compute path length (mm) from currentPos to adjusted target
  float dx = adjusted.x - currentPos->x;
  float dy = adjusted.y - currentPos->y;
  float dz = adjusted.z - currentPos->z;
  float path_len = sqrtf(dx*dx + dy*dy + dz*dz);
    {
      (void)path_len;
      (void)feed_mm_per_min;
    }
    // Delegate to motion layer which handles coordinated stepping and pacing
  moveLinearMotorSteps_timed(currentMotorSteps, &targetSteps, pulseStepFunc, setDirFunc,
                            // delayFunc: forwarded from caller
                            delayFunc, feed_mm_per_min, path_len);
    (void)currentMotorSteps;
}
