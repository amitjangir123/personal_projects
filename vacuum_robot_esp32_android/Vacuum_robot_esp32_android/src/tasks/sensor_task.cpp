#include <Arduino.h>
#include "common/robot_state.h"
#include "common/config.h"
#include "sensor_task.h"

//HardwareSerial uart_port(2); // Use UART2 (pins 16 and 17) for Serial2

void initSensors()
{
    pinMode(Config::BUMPER_PIN, INPUT_PULLUP);
    pinMode(Config::IR_SENSOR_PIN, INPUT_PULLUP);
    pinMode(Config::CHARGING_SENSOR_PIN, INPUT_PULLDOWN);
    //uart_port.begin(115200, SERIAL_8N1, Config::UART_RX_PIN, Config::UART_TX_PIN); // Initialize Serial2 with RX on GPIO 16 and TX on GPIO 17
    //pinMode(Config::UART_RX_PIN, OUTPUT); // Set the built-in LED pin as output
    //pinMode(4, OUTPUT); // Set the built-in LED pin as output
}


void sensorsTask(void *parameter)
{
    RobotState::state.currRobotMode = RobotState::state.DOCKING; // Start in docking mode
    while (true)
    {
        bool bumperPressed = digitalRead(Config::BUMPER_PIN);

        //Serial.printf("Sensors Task: Bumper State: %x", bumperPressed);
        if(bumperPressed)
        {
            vTaskDelay(pdMS_TO_TICKS(20)); 
            if(digitalRead(Config::BUMPER_PIN))
            {
                RobotState::state.isBumperPressed = bumperPressed;
            }
        }
        else
        {
            RobotState::state.isBumperPressed = false;
        }
          

        bool irSensorActive = digitalRead(Config::IR_SENSOR_PIN); // Read the state of the bumper sensor (active low) 
        RobotState::state.isIRSensorActive = irSensorActive; // Update the robot state with the bumper sensor state 

        bool chargingSensorActive = digitalRead(Config::CHARGING_SENSOR_PIN); // Read the state of the charging sensor (active high)
        RobotState::state.isCharging = chargingSensorActive; // Update the robot state with the charging sensor state  

        //Serial2.println("H");
        // Serial2.println("L:" + String(1000) + " LS:" + String(15000) + " R:" + String(1000) + " RS:" + String(15000));
        // Serial.println("L:" + String(1000) + " LS:" + String(15000) + " R:" + String(1000) + " RS:" + String(15000));
        

        // if(RobotState::state.isCharging)
        // {
        //     Serial.printf("Sensors Task: Charging Sensor Active\n");
        // }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
