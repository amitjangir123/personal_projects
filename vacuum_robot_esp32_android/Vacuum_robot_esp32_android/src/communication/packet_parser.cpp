#include <Arduino.h>

#include "packet_parser.h"
#include "common/robot_state.h"

namespace PacketParser
{

void parse(const char* packet)
{
    if (packet[0] == 'H')
    {
        float heading = atof(packet + 2); // Convert the string to float
        if(heading < 0.0f)
        {
            heading += 360.0f; // Normalize negative headings to positive
        }
        RobotState::state.currentHeadingDegrees = heading; // Update the robot's heading
        //Serial.printf("Heading = ");
        Serial.println(RobotState::state.currentHeadingDegrees);
    }
}

}