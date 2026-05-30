#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Convert Cartesian (X, Y, Z) to motor coordinates (A, B, Z)
MotorSteps cartesianToMotors(float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif
