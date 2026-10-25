#include <Arduino.h>
#include "docking_task.h"
#include "common/robot_state.h"
#include "common/common_functions.h"
#include "motion/motion.h"
#include "common/config.h"

uint32_t get_ir_heading(uint32_t minIndex, uint32_t maxIndex);
void get_rough_ir_heading(uint32_t *minIndex, uint32_t *maxIndex);
uint32_t irHeading = 0; // Global variable to store the IR heading
uint32_t minIndex = 0; // Global variable to store the minimum index of the IR heading
uint32_t maxIndex = 360; // Global variable to store the maximum index of the IR heading

void dockingTask(void *parameter)
{
    (void)parameter;
    enum dockingState{
        IDLE,
        SEARCHING_FOR_DOCK,
        ALIGNING_WITH_DOCK,
        DOCKING
    };

    dockingState dockingState = SEARCHING_FOR_DOCK; // Start with searching for the dock

    //Stop motors before starting the docking task
    Motion::motorStop();

    Serial.println("Docking Task Started");

    while (true)
    {
        if(RobotState::state.currRobotMode != RobotState::state.DOCKING)
        {
            myDelay(1000);
            continue;
        }
        switch(dockingState)
        {
            case IDLE:
            {
                // Stop all motors
                Motion::motorStop();
            }
            break;
            case SEARCHING_FOR_DOCK:
            {
                
                minIndex = 0;
                maxIndex = 360;
                get_rough_ir_heading(&minIndex, &maxIndex); // Get the rough heading of the dock
                irHeading = get_ir_heading(minIndex, maxIndex); // Scan from minIndex to maxIndex degrees to find the IR heading
                Serial.println("IR Heading: " + String(irHeading));
                //myDelay(5000); // Wait for a second before the next scan
                Motion::motorTurn(200, irHeading); // Turn to the IR heading
                // Wait for the turn to complete
                while(!Motion::motorWaitForIDLE())
                {
                    myDelay(20);
                }
                Serial.println("Driving forward for a set number of steps...");
                while(1)
                {
                    myDelay(50); // Wait for a short duration to allow the IR sensor to stabilize
                    if(RobotState::state.isCharging)
                    {
                        Serial.println("Docking Successful!");
                        dockingState = IDLE; // Docking successful, go to IDLE state

                    }
                    else
                    {
                        myDelay(100); // Wait for a short duration to allow the IR sensor to stabilize
                        if(RobotState::state.isIRSensorActive)
                        {
                            Serial.println("Docking Failed! Retrying...");
                                Motion::motorStartSteps(200, 100, 100); // Move forward for 100 steps
                                while(!Motion::motorWaitForIDLE())
                                {
                                    myDelay(20);
                                }
                                //Motion::motorTurn(200, irHeading); // Turn to the IR heading
                                // while(!Motion::motorWaitForIDLE())
                                // {
                                //     myDelay(20);
                                // }
                        }
                        else
                        {
                            break; // Exit the loop if the IR sensor is not active
                        }

                    }
                }
                
            }
            break;
            case ALIGNING_WITH_DOCK:
            {
                // Implement aligning with the dock behavior here
            }
            default:
                break;
        }
        // Docking behavior will be implemented here.
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void get_rough_ir_heading(uint32_t *minIndex, uint32_t *maxIndex)
{
    bool irDataArray[360] = {false}; // Initialize all to false
    bool high_flag = 0;
    uint8_t low_flag = 0;

    Motion::motorTurn(500, *minIndex); // Turn to heading 0 degrees
    // Wait for the turn to complete
    while(!Motion::motorWaitForIDLE())
    {
        myDelay(20);
    }

    for(int i = *minIndex; i < *maxIndex; i+=10)
    {
        Serial.println("Scanning Heading: " + String(i) + " degrees");
        Serial.println("Current Heading: " + String(RobotState::state.currentHeadingDegrees));
        Motion::motorTurn(500, i); // Turn to heading 0 degrees
        // Wait for the turn to complete
        while(!Motion::motorWaitForIDLE())
        {
            myDelay(20);
        }
        delay(1); // Wait for a short duration to allow the IR sensor to stabilize 
        irDataArray[i] = digitalRead(Config::IR_SENSOR_PIN); // Mark this heading as scanned
        
        Serial.println("irDataArray[" + String(i) + "]: " + String(irDataArray[i]));

        if(irDataArray[i])
        {
            high_flag = 1;
            low_flag = 0;
        }
        else if (!irDataArray[i] && high_flag == 1)
            low_flag++;

        if(low_flag > 5)
        {
            Serial.println("IR Sensor Inactive for 5 consecutive readings. Stopping scan.");
            break; // Exit the loop if the IR sensor is inactive for 5 consecutive readings
        }
    }
    
    //Find Minimum heading with IR sensor active
    for (int i = 0; i < 360; ++i)
    {
        if(irDataArray[i])
        {
            *minIndex = i;
            break; // Exit the loop once the first active heading is found
        }
    }

    //Find Maximum heading with IR sensor active
    for (int i = 359; i >= 0; --i)
    {
        if(irDataArray[i])
        {
            *maxIndex = i;
            break; // Exit the loop once the last active heading is found
        }
    }
}

uint32_t get_ir_heading(uint32_t minIndex = 0, uint32_t maxIndex = 360)
{
    bool irDataArray[360] = {false}; // Initialize all to false
    bool high_flag = 0;
    uint8_t low_flag = 0;

    Motion::motorTurn(500, minIndex); // Turn to heading 0 degrees
    // Wait for the turn to complete
    while(!Motion::motorWaitForIDLE())
    {
        myDelay(20);
    }

    for(int i = minIndex; i < maxIndex; i++)
    {
        Serial.println("Scanning Heading: " + String(i) + " degrees");
        Serial.println("Current Heading: " + String(RobotState::state.currentHeadingDegrees));
        Motion::motorTurn(100, i); // Turn to heading 0 degrees
        // Wait for the turn to complete
        while(!Motion::motorWaitForIDLE())
        {
            myDelay(20);
        }
        delay(1); // Wait for a short duration to allow the IR sensor to stabilize 
        irDataArray[i] = digitalRead(Config::IR_SENSOR_PIN); // Mark this heading as scanned
        
        Serial.println("irDataArray[" + String(i) + "]: " + String(irDataArray[i]));

        if(irDataArray[i])
        {
            high_flag = 1;
            low_flag = 0;
        }
        else if (!irDataArray[i] && high_flag == 1)
            low_flag++;

        if(low_flag > 10)
        {
            Serial.println("IR Sensor Inactive for 5 consecutive readings. Stopping scan.");
            break; // Exit the loop if the IR sensor is inactive for 5 consecutive readings
        }
    }
    
    //Find Minimum heading with IR sensor active
    for (int i = 0; i < 360; ++i)
    {
        if(irDataArray[i])
        {
            minIndex = i;
            break; // Exit the loop once the first active heading is found
        }
    }

    //Find Maximum heading with IR sensor active
    for (int i = 359; i >= 0; --i)
    {
        if(irDataArray[i])
        {
            maxIndex = i;
            break; // Exit the loop once the last active heading is found
        }
    }

    Serial.println("Minimum Heading with IR Active: " + String(minIndex));
    Serial.println("Maximum Heading with IR Active: " + String(maxIndex));

    //myDelay(2000); // Wait for a second before calculating the average heading

    // Consider wrap-around case first
    if(minIndex == 0 && maxIndex == 359)
    {
        uint32_t lowestActive_of_upperRange = 0;
        uint32_t highestActive_of_lowerRange = 0;
        for (int i = 0; i < 360; ++i)
        {
            if(irDataArray[i] == false)
            {
                highestActive_of_lowerRange = i - 1;
                break; // Exit the loop once the first active heading is found
            }
        }

        for (int i = 359; i >= 0; --i)
        {
            if(irDataArray[i] == false)
            {
                lowestActive_of_upperRange = i + 1;
                break; // Exit the loop once the last active heading is found
            }
        }

        // Calculate the average heading considering wrap-around
        float averageHeading = (highestActive_of_lowerRange + lowestActive_of_upperRange + 360) / 2.0f;
        if(averageHeading >= 360.0f)
        {
            averageHeading -= 360.0f;
        }
        return averageHeading;

    }
    else
    {
        // Calculate the average heading
        float averageHeading = (minIndex + maxIndex) / 2.0f;
        return averageHeading;
    }
}
