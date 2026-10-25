#include <Arduino.h>
#include "motion.h"
#include "common/robot_state.h"
#include "common/config.h"
#include "common/common_functions.h"
#include "tasks/motion_task.h"

namespace Motion {
    
    void motorStop(void)
    {
        RobotState::state.currMotionCmd = RobotState::State::IDLE;

    }
    void motorStart(float targetSpeed, float targetHeadingDegrees)
    {
        RobotState::state.targetHeadingDegrees = targetHeadingDegrees;
        RobotState::state.currMotionCmd = RobotState::State::FORWARD;

    }
    void motorStartSteps(float targetSpeed, uint32_t leftSteps, uint32_t rightSteps)
    {
        RobotState::state.leftWheelSteps = leftSteps;
        RobotState::state.rightWheelSteps = rightSteps;
        RobotState::state.leftWheelSpeed = targetSpeed;
        RobotState::state.rightWheelSpeed = targetSpeed;
        RobotState::state.currMotionCmd = RobotState::State::FORWARD_STEPS;

    }
    void motorBackwardStep(uint32_t steps)
    {
        RobotState::state.currMotionCmd = RobotState::State::BACKWARD;
        RobotState::state.leftWheelSteps = steps;
        RobotState::state.rightWheelSteps = steps;

    }
    void motorTurn(float targetSpeed, float targetHeadingDegrees)
    {
        RobotState::state.targetHeadingDegrees = targetHeadingDegrees;
        RobotState::state.leftWheelSpeed = targetSpeed;
        RobotState::state.rightWheelSpeed = targetSpeed;
        RobotState::state.currMotionCmd = RobotState::State::TURN;

    }
    bool motorWaitForIDLE(void)
    {   
        myDelay(20);
        if(RobotState::state.currMotionCmd != RobotState::State::IDLE)
        {
            return false;
        }
        return true;
    }
}
