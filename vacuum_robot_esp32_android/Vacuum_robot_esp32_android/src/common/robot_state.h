#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include "tasks/motion_task.h" // Include MotionMode enum

namespace RobotState
{
    struct State
    {
        float currentHeadingDegrees = 0.0f;
        float targetHeadingDegrees = 0.0f;
        uint32_t leftWheelSpeed = 0;
        uint32_t rightWheelSpeed = 0;
        uint32_t leftWheelSteps = 0;
        uint32_t rightWheelSteps = 0;
        bool isBumperPressed = false;
        bool isIRSensorActive = false;
        bool isCharging = false;

        enum RobotMode
        {
            CLEANING,
            DOCKING
        };
        RobotMode currRobotMode = CLEANING;

        enum currMotionCmd
        {
            IDLE,
            FORWARD,
            FORWARD_STEPS,
            BACKWARD,
            TURN
        };
        currMotionCmd currMotionCmd = IDLE;

    };
    extern State state;
}

#endif