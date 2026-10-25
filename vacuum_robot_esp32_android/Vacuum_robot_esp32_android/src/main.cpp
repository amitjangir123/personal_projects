#include <Arduino.h>
#include "tasks/communication_task.h"
#include "tasks/motion_task.h"
#include "tasks/cleaning_task.h"
#include "common/config.h"
#include "tasks/sensor_task.h"
#include "common/robot_state.h"
#include "tasks/docking_task.h"


void setup() 
{
  //MotorController::init();
  Serial.begin(115200);
  Serial2.begin(Config::UART_BAUD_RATE, SERIAL_8N1, Config::UART_RX_PIN, Config::UART_TX_PIN);

  Serial.println("Robot Starting...");

  delay(1000); // Wait for 1 seconds to ensure everything is initialized

  
  xTaskCreatePinnedToCore(
    communicationTask,   // Task function
    "CommunicationTask", // Name of the task
    4096,               // Stack size (bytes)
    NULL,                // Parameter to pass to the task
    1,                   // Task priority
    NULL,                // Task handle
    0                    // Core to run the task on (0 or 1)
  );
  

  delay(5000); // Small delay to ensure the first task starts before the second

  xTaskCreatePinnedToCore(
    motionTask,         // Task function
    "MotionTask",       // Name of the task
    4096,               // Stack size (bytes)
    NULL,                // Parameter to pass to the task
    2,                  // Task priority higher than communication task
    NULL,                // Task handle
    1                   // Core to run the task on (0 or 1)
  );

  
  xTaskCreatePinnedToCore(
    cleaningTask,       // Task function
    "CleaningTask",     // Name of the task
    4096,               // Stack size (bytes)
    NULL,                // Parameter to pass to the task
    2,                   // Task priority
    NULL,                // Task handle
    1                    // Core to run the task on (0 or 1)
  );

  xTaskCreatePinnedToCore(
    dockingTask,       // Task function
    "DockingTask",     // Name of the task
    4096,               // Stack size (bytes)
    NULL,                // Parameter to pass to the task
    2,                   // Task priority
    NULL,                // Task handle
    1                    // Core to run the task on (0 or 1)
  );
  
  xTaskCreatePinnedToCore(
    sensorsTask,        // Task function
    "SensorsTask",      // Name of the task
    4096,               // Stack size (bytes)
    NULL,                // Parameter to pass to the task
    2,                   // Task priority
    NULL,                // Task handle
    1                    // Core to run the task on (0 or 1)
  );

  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.printf("temp1: %f\n", RobotState::state.currentHeadingDegrees);

}
