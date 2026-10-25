#include <Arduino.h>
#include "cleaning_task.h"
#include "common/robot_state.h"
#include "common/common_functions.h"
#include "motion/motion.h"

void cleaningTask(void *pvParameters)
{
    enum cleaningState{ 
        IDLE,
        DRIVE_FORWARD,
        DRIVE_BACKWARD,
        TURNING,
        DRIVE_FORWARD_STEPS};

    //navState navState = IDLE;
    cleaningState cleaningState = DRIVE_FORWARD; // Start with driving forward for testing

    uint8_t turnCounter = 0;

    float originalHeading = RobotState::state.currentHeadingDegrees;
    while (true)
    {
        if (RobotState::state.currRobotMode != RobotState::state.CLEANING)
        {
            myDelay(1000);
            continue;
        }
        switch(cleaningState)
        {
            case IDLE:
            {
                Motion::motorStop();
                // if(startCleaning)
                //     navState = DRIVE_FORWARD;
            }
            break;
            case DRIVE_FORWARD:
            {
                Motion::motorStart(2000, originalHeading);
                Serial.println("Driving forward...");
                while(!RobotState::state.isBumperPressed)
                {
                    //DONT NOT REMOVE THIS DELAY, IT IS NEEDED TO ALLOW THE TASK TO YIELD AND CHECK FOR BUMPER PRESS
                    myDelay(20);
                }
                Serial.println("Bumper pressed! Stopping and reversing...");
                Motion::motorStop();
                cleaningState = DRIVE_BACKWARD;
            }
            break;
            case DRIVE_BACKWARD:
            {
                Serial.println("Driving backward...");
                myDelay(500);
                Motion::motorBackwardStep(1000); // 1000 steps backward
                // Wait for the movement to complete
                while(!Motion::motorWaitForIDLE())
                {
                    myDelay(20);
                }
                //navState = IDLE;
                cleaningState = TURNING;
            }
            break;
            case TURNING:
            {
                switch(turnCounter)
                {
                    case 0:
                        Serial.println("Case 0:Turning 90 degrees to the right...");
                        originalHeading = originalHeading + 90.0f; // Turn 90 degrees to the right
                        originalHeading = normalizeHeading(originalHeading);
                        Serial.println("New Heading: " + String(originalHeading));
                        Motion::motorTurn(2000, originalHeading);
                        // Wait for the turn to complete
                        while(!Motion::motorWaitForIDLE())
                        {
                            myDelay(20);
                        }
                        turnCounter++;
                        cleaningState = DRIVE_FORWARD_STEPS;
                        break;
                    case 1:
                        Serial.println("Case 1:Turning 90 degrees to the right...");
                        originalHeading = originalHeading + 90.0f; // Turn 90 degrees to the right
                        originalHeading = normalizeHeading(originalHeading);
                        Serial.println("New Heading: " + String(originalHeading));
                        Motion::motorTurn(2000, originalHeading);
                        // Wait for the turn to complete
                        while(!Motion::motorWaitForIDLE())
                        {
                            myDelay(20);
                        }
                        turnCounter++;
                        cleaningState = DRIVE_FORWARD; 
                        break;
                    case 2:
                        Serial.println("Case 2:Turning 90 degrees to the left...");
                        originalHeading = originalHeading - 90.0f; // Turn 90 degrees to the left
                        originalHeading = normalizeHeading(originalHeading);
                        Serial.println("New Heading: " + String(originalHeading));
                        Motion::motorTurn(2000, originalHeading);
                        // Wait for the turn to complete
                        while(!Motion::motorWaitForIDLE())
                        {
                            myDelay(20);
                        }
                        turnCounter++;
                        cleaningState = DRIVE_FORWARD_STEPS; 
                        break;
                    case 3:
                        Serial.println("Case 3:Turning 90 degrees to the left...");
                        originalHeading = originalHeading - 90.0f; // Turn 90 degrees to the left
                        originalHeading = normalizeHeading(originalHeading);
                        Serial.println("New Heading: " + String(originalHeading));
                        Motion::motorTurn(2000, originalHeading);
                        // Wait for the turn to complete
                        while(!Motion::motorWaitForIDLE())
                        {
                            myDelay(20);
                        }
                        turnCounter = 0; // Reset turn counter
                        cleaningState = DRIVE_FORWARD;
                        break;
                    }
            }
            break;
            case DRIVE_FORWARD_STEPS:
            {
                Serial.println("Driving forward for a set number of steps...");
                Motion::motorStartSteps(2000, 1000, 1000); // Drive forward for 1000 steps
                // Wait for the movement to complete
                while(!Motion::motorWaitForIDLE())
                {
                    myDelay(20);
                }
                cleaningState = TURNING; // After moving forward, go to turning state

            }
            break;
            default:
                cleaningState = IDLE;

        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    
}
