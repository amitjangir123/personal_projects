#include "kinematics.h"
#include <math.h>

/**
 * cartesianToMotors - Convert Cartesian coordinates to CoreXY motor steps
 * 
 * CoreXY is a kinematic system where two motors (A and B) work together to move in X and Y.
 * Instead of one motor per axis, CoreXY distributes the motion:
 * 
 * CoreXY Equations:
 *   Motor_A = X + Y    (Motor A pulls diagonally up-right)
 *   Motor_B = X - Y    (Motor B pulls diagonally down-right)
 *   Motor_Z = Z        (Z-axis direct, no coupling)
 * 
 * This allows faster diagonal motion since both motors can move simultaneously
 * without losing precision.
 * 
 * Example 1 - Move right (X+):
 *   Input: X=10, Y=0
 *   Motor_A = 10 + 0 = 10 steps
 *   Motor_B = 10 - 0 = 10 steps
 *   Result: Both motors rotate, belt mechanics move bed right
 * 
 * Example 2 - Move up (Y+):
 *   Input: X=0, Y=10
 *   Motor_A = 0 + 10 = 10 steps
 *   Motor_B = 0 - 10 = -10 steps
 *   Result: Motors rotate opposite directions, belt mechanics move bed forward
 * 
 * Example 3 - Move diagonally (X+, Y+):
 *   Input: X=10, Y=10
 *   Motor_A = 10 + 10 = 20 steps
 *   Motor_B = 10 - 10 = 0 steps
 *   Result: Only motor A moves (fastest diagonal motion!)
 * 
 * @param x: X coordinate in mm
 * @param y: Y coordinate in mm
 * @param z: Z coordinate in mm
 * @return: MotorSteps struct with motorA, motorB, motorZ step counts
 */
MotorSteps cartesianToMotors(float x, float y, float z) {
  MotorSteps steps = {0};
  
  // Convert coordinates to steps
  // Using STEPS_PER_MM constants from config.h
  // These define motor resolution (e.g., 80 steps per mm)
  
  float x_steps = x * STEPS_PER_MM_X;  // Convert X position (mm) to steps
  float y_steps = y * STEPS_PER_MM_Y;  // Convert Y position (mm) to steps
  float z_steps = z * STEPS_PER_MM_Z;  // Convert Z position (mm) to steps
  
  // CoreXY transformation:
  // Motor A controls X+Y direction
  steps.motorA = (int)round(x_steps + y_steps);
  
  // Motor B controls X-Y direction
  steps.motorB = (int)round(x_steps - y_steps);
  
  // Z is direct (no coupling)
  steps.motorZ = (int)round(z_steps);
  
  return steps;
}

/**
 * Inverse Kinematics (NOT IMPLEMENTED - for reference)
 * 
 * If you need to convert motor steps back to Cartesian coordinates:
 *   X = (Motor_A + Motor_B) / 2
 *   Y = (Motor_A - Motor_B) / 2
 * 
 * This could be useful for position tracking or error checking.
 */
