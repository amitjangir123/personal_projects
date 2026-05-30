#include "motion.h"
#include <stdint.h>
#include <stdlib.h>

// Bresenham-style interpolator across three motors (A, B, Z)
// current and target are absolute step counts for each motor.
void moveLinearMotorSteps(const MotorSteps* current, const MotorSteps* target,
                          void (*pulseStepFunc)(int),
                          void (*setDirFunc)(int, int)) {
  int32_t a_cur = current->motorA;
  int32_t b_cur = current->motorB;
  int32_t z_cur = current->motorZ;

  int32_t a_tgt = target->motorA;
  int32_t b_tgt = target->motorB;
  int32_t z_tgt = target->motorZ;

  int32_t da = a_tgt - a_cur;
  int32_t db = b_tgt - b_cur;
  int32_t dz = z_tgt - z_cur;

  int dirA = (da >= 0) ? 1 : 0;
  int dirB = (db >= 0) ? 1 : 0;
  int dirZ = (dz >= 0) ? 1 : 0;

  int32_t absA = (da >= 0) ? da : -da;
  int32_t absB = (db >= 0) ? db : -db;
  int32_t absZ = (dz >= 0) ? dz : -dz;

  // Set direction pins
  setDirFunc(MOTOR_A_DIR_PIN, dirA);
  setDirFunc(MOTOR_B_DIR_PIN, dirB);
  setDirFunc(MOTOR_Z_DIR_PIN, dirZ);

  int32_t maxSteps = absA;
  if (absB > maxSteps) maxSteps = absB;
  if (absZ > maxSteps) maxSteps = absZ;

  if (maxSteps == 0) return; // nothing to do

  int64_t errA = 0, errB = 0, errZ = 0;

  for (int32_t i = 0; i < maxSteps; i++) {
    if (absA > 0) {
      errA += absA;
      if (errA >= maxSteps) {
        pulseStepFunc(MOTOR_A_STEP_PIN);
        errA -= maxSteps;
      }
    }

    if (absB > 0) {
      errB += absB;
      if (errB >= maxSteps) {
        pulseStepFunc(MOTOR_B_STEP_PIN);
        errB -= maxSteps;
      }
    }

    if (absZ > 0) {
      errZ += absZ;
      if (errZ >= maxSteps) {
        pulseStepFunc(MOTOR_Z_STEP_PIN);
        errZ -= maxSteps;
      }
    }
  }
}

void moveLinearMotorSteps_timed(const MotorSteps* current, const MotorSteps* target,
                               void (*pulseStepFunc)(int),
                               void (*setDirFunc)(int, int),
                               void (*delayFunc)(unsigned int),
                               float feed_mm_per_min,
                               float path_length_mm) {
  // If no feedrate provided or no delay function, fall back to untimed behavior
  if (feed_mm_per_min <= 0.0f || delayFunc == NULL) {
    moveLinearMotorSteps(current, target, pulseStepFunc, setDirFunc);
    return;
  }

  int32_t a_cur = current->motorA;
  int32_t b_cur = current->motorB;
  int32_t z_cur = current->motorZ;

  int32_t a_tgt = target->motorA;
  int32_t b_tgt = target->motorB;
  int32_t z_tgt = target->motorZ;

  int32_t da = a_tgt - a_cur;
  int32_t db = b_tgt - b_cur;
  int32_t dz = z_tgt - z_cur;

  int dirA = (da >= 0) ? 1 : 0;
  int dirB = (db >= 0) ? 1 : 0;
  int dirZ = (dz >= 0) ? 1 : 0;

  int32_t absA = (da >= 0) ? da : -da;
  int32_t absB = (db >= 0) ? db : -db;
  int32_t absZ = (dz >= 0) ? dz : -dz;

  // Set direction pins
  setDirFunc(MOTOR_A_DIR_PIN, dirA);
  setDirFunc(MOTOR_B_DIR_PIN, dirB);
  setDirFunc(MOTOR_Z_DIR_PIN, dirZ);

  int32_t maxSteps = absA;
  if (absB > maxSteps) maxSteps = absB;
  if (absZ > maxSteps) maxSteps = absZ;

  if (maxSteps == 0) return; // nothing to do
    {
      (void)maxSteps; (void)absA; (void)absB; (void)absZ; (void)path_length_mm; (void)feed_mm_per_min;
    }

  // Compute total time for the move in microseconds
  // feed_mm_per_min is mm per minute; convert to mm per microsecond: / (60*1e6)
  double feed_mm_per_us = (double)feed_mm_per_min / 60000000.0; // mm/us
  double total_time_us = 0.0;
  if (path_length_mm > 0.0) {
    total_time_us = (double)path_length_mm / feed_mm_per_us; // us
  }

  // Use uniform per-step pacing (no acceleration/deceleration)
  double base_us = 0.0;
  if (total_time_us > 0.0 && maxSteps > 0) {
    base_us = total_time_us / (double)maxSteps;
  }

  int64_t errA = 0, errB = 0, errZ = 0;

  for (int32_t i = 0; i < maxSteps; i++) {
    if (absA > 0) {
      errA += absA;
      if (errA >= maxSteps) {
        pulseStepFunc(MOTOR_A_STEP_PIN);
        errA -= maxSteps;
      }
    }

    if (absB > 0) {
      errB += absB;
      if (errB >= maxSteps) {
        pulseStepFunc(MOTOR_B_STEP_PIN);
        errB -= maxSteps;
      }
    }

    if (absZ > 0) {
      errZ += absZ;
      if (errZ >= maxSteps) {
        pulseStepFunc(MOTOR_Z_STEP_PIN);
        errZ -= maxSteps;
      }
    }

    // Pace the loop to match requested feedrate (uniform timing)
    if (base_us > 0.5) { // avoid tiny delays
      double usd = base_us;
      unsigned int us = (unsigned int)(usd + 0.5);
      delayFunc(us);
    }
  }
  (void)maxSteps;
}

// Variant that pulses multiple step pins simultaneously using a bitmask
// pulseMaskFunc(bitmask): bit i corresponds to pin i (1u<<pin).
void moveLinearMotorSteps_mask(const MotorSteps* current, const MotorSteps* target,
                               void (*pulseMaskFunc)(unsigned int),
                               void (*setDirFunc)(int, int)) {
  int32_t a_cur = current->motorA;
  int32_t b_cur = current->motorB;
  int32_t z_cur = current->motorZ;

  int32_t a_tgt = target->motorA;
  int32_t b_tgt = target->motorB;
  int32_t z_tgt = target->motorZ;

  int32_t da = a_tgt - a_cur;
  int32_t db = b_tgt - b_cur;
  int32_t dz = z_tgt - z_cur;

  int dirA = (da >= 0) ? 1 : 0;
  int dirB = (db >= 0) ? 1 : 0;
  int dirZ = (dz >= 0) ? 1 : 0;

  uint32_t maskA = (da != 0) ? (1u << MOTOR_A_STEP_PIN) : 0u;
  uint32_t maskB = (db != 0) ? (1u << MOTOR_B_STEP_PIN) : 0u;
  uint32_t maskZ = (dz != 0) ? (1u << MOTOR_Z_STEP_PIN) : 0u;

  int32_t absA = (da >= 0) ? da : -da;
  int32_t absB = (db >= 0) ? db : -db;
  int32_t absZ = (dz >= 0) ? dz : -dz;

  // Set direction pins
  setDirFunc(MOTOR_A_DIR_PIN, dirA);
  setDirFunc(MOTOR_B_DIR_PIN, dirB);
  setDirFunc(MOTOR_Z_DIR_PIN, dirZ);

  int32_t maxSteps = absA;
  if (absB > maxSteps) maxSteps = absB;
  if (absZ > maxSteps) maxSteps = absZ;

  if (maxSteps == 0) return;

  int64_t errA = 0, errB = 0, errZ = 0;

  for (int32_t i = 0; i < maxSteps; i++) {
    unsigned int toPulse = 0u;
    if (absA > 0) {
      errA += absA;
      if (errA >= maxSteps) { toPulse |= maskA; errA -= maxSteps; }
    }
    if (absB > 0) {
      errB += absB;
      if (errB >= maxSteps) { toPulse |= maskB; errB -= maxSteps; }
    }
    if (absZ > 0) {
      errZ += absZ;
      if (errZ >= maxSteps) { toPulse |= maskZ; errZ -= maxSteps; }
    }

    if (toPulse) {
      pulseMaskFunc(toPulse);
    }
  }
}
