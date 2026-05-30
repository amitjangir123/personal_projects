#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <Arduino.h>

// Pin definitions (board-specific)
// Motor A
#define MOTOR_A_STEP_PIN PB4
#define MOTOR_A_DIR_PIN  PB3
#define MOTOR_A_EN_PIN   PB5

// Motor B
#define MOTOR_B_STEP_PIN PB13
#define MOTOR_B_DIR_PIN  PB12
#define MOTOR_B_EN_PIN   PB14

// Motor Z
#define MOTOR_Z_STEP_PIN PC14
#define MOTOR_Z_DIR_PIN  PC13
#define MOTOR_Z_EN_PIN   PC15

// Backlash compensation (in millimeters)
// Set backlash in mm for each axis. Previously this was specified in steps;
// using mm here is more intuitive and independent from steps/mm settings.
// Defaults match previous step-based values: X/Y = 50 steps @80 steps/mm = 0.625mm
// Z = 20 steps @400 steps/mm = 0.05mm
#define BACKLASH_X_MM 0.2
#define BACKLASH_Y_MM 0.05
#define BACKLASH_Z_MM 0.0

// Motion constants
#define STEPS_PER_MM_X 162
#define STEPS_PER_MM_Y 162
#define STEPS_PER_MM_Z 2000

// Maximum allowed feedrate (mm per minute). G-code feedrates above this
// value will be clamped to this maximum. Adjust to your machine's capability.
#define MAX_FEEDRATE 1000.0f

// Acceleration/profile tuning removed: uniform pacing is used instead

// Data structures
typedef struct {
  float x, y, z;
} Position;

typedef struct {
  int32_t motorA, motorB, motorZ;
} MotorSteps;

typedef struct {
  char command;     // 'G', 'M'
  int code;         // numerical code
  float x, y, z, f; // parameters
  int hasX, hasY, hasZ, hasF; // flags
} GcodeCommand;

typedef struct {
  int x_dir, y_dir, z_dir;  // 1 or -1
} DirectionState;

#endif
