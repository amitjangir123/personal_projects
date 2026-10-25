#include <Arduino.h>
#include "tasks/motion_task.h"
#include "common/common_functions.h"
#include "common/robot_state.h"
#include "common/config.h"

float kP = 100.0f; // Proportional gain for heading
const float ROTATE_ERROR = 1.0f; // Degrees

uint32_t max_speed = Config::MAX_SPEED;

void moveforward_maintain_heading(float targetSpeed, float targetHeadingDegrees);
void rotate_to_heading(float targetSpeed, float targetHeadingDegrees);

void motionTask(void *parameter)
{

    while(true)
    {
        switch (RobotState::state.currMotionCmd)
        {
        case RobotState::State::IDLE:
        {
            //Send stop command to STM32
            sendStm32Cmd(0, 0, 0, 0);
        }
        break;
        case RobotState::State::FORWARD:
        {
            moveforward_maintain_heading(2000, RobotState::state.targetHeadingDegrees);
            //sendStm32Cmd(2000, 15000, 2000, 15000);
            //myDelay(2000);
            //RobotState::state.currMotionCmd = RobotState::State::IDLE;
        }
        break;
        case RobotState::State::FORWARD_STEPS:
        {
            sendStm32Cmd(RobotState::state.leftWheelSpeed,
                         RobotState::state.leftWheelSteps,
                         RobotState::state.rightWheelSpeed,
                         RobotState::state.rightWheelSteps);
            myDelay(2000);
            RobotState::state.currMotionCmd = RobotState::State::IDLE;
        }
        break;
        case RobotState::State::BACKWARD:
        {
            sendStm32Cmd(-1000, RobotState::state.leftWheelSteps, -1000, RobotState::state.rightWheelSteps);
            myDelay(2000);
            RobotState::state.currMotionCmd = RobotState::State::IDLE;

        }
        break;
        case RobotState::State::TURN:
        {
            //Adding slight delay so that IR measurement can be started before the robot starts turning. This is to avoid missing the IR signal during the turn.
            myDelay(100); 
            rotate_to_heading(RobotState::state.leftWheelSpeed, RobotState::state.targetHeadingDegrees);
        }
        break;
        
        
        default:
            break;
        }
        myDelay(20);
    }
}

void moveforward_maintain_heading(float targetSpeed, float targetHeadingDegrees)
{
    float headingError = targetHeadingDegrees - RobotState::state.currentHeadingDegrees;
    float correction = kP * headingError;

    uint32_t leftWheelSpeed = targetSpeed + correction;
    uint32_t rightWheelSpeed = targetSpeed - correction;

    // Clamp wheel speeds to the maximum speed
    if (leftWheelSpeed > max_speed) leftWheelSpeed = max_speed;
    if (rightWheelSpeed > max_speed) rightWheelSpeed = max_speed;

    sendStm32Cmd(leftWheelSpeed, 15000, rightWheelSpeed, 15000);
}

void rotate_to_heading(float targetSpeed, float targetHeadingDegrees)
{
   
    // Normalize the heading error to be within -180 to 180 degrees
    float error = headingError(targetHeadingDegrees, RobotState::state.currentHeadingDegrees);
    //Serial.printf("Rotate to Heading: Target: %.2f, Current: %.2f, Error: %.2f\n", targetHeadingDegrees, RobotState::state.currentHeadingDegrees, error);

    if (fabs(error) > ROTATE_ERROR)
    {
        // Determine the direction to turn
        int32_t leftWheelSpeed = (error > 0) ? targetSpeed : -targetSpeed;
        int32_t rightWheelSpeed = (error > 0) ? -targetSpeed : targetSpeed;

        sendStm32Cmd(leftWheelSpeed, 15000, rightWheelSpeed, 15000);
    }
    else
    {
        // Stop the motors when the heading is within the acceptable error range
        sendStm32Cmd(0, 0, 0, 0);
        RobotState::state.currMotionCmd = RobotState::State::IDLE;
        Serial.println("Rotation complete. Heading aligned.");
    }
}

