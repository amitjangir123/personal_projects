#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>

namespace Motion
{
    void moveForward(float targetSpeed, float targetHeadingDegrees);
    void moveBackward(float targetSpeed, float targetHeadingDegrees);
    bool rotateToHeading(float targetSpeed, float targetHeadingDegrees);

    void motorStart(float targetSpeed, float targetHeadingDegrees);
    void motorStartSteps(float targetSpeed, uint32_t leftSteps, uint32_t rightSteps);
    void motorStop(void);
    void motorBackwardStep(uint32_t steps);
    void motorTurn(float targetSpeed, float targetHeadingDegrees);
    bool motorWaitForIDLE(void);
} // namespace Motion


#endif // MOTION_H