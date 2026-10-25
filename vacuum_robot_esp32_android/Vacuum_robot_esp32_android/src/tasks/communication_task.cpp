#include "communication_task.h"
#include <Arduino.h>
#include "communication/wifi_manager.h"
#include "communication/udp_manager.h"
#include "communication/packet_parser.h"

void communicationTask(void *pvParameters)
{
    WiFiManager::connect(); // Connect to WiFi

    if (!UDPManager::begin())
    {
        Serial.println("UDP initialization failed!");
        while(true)
        {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }

    }

    char rxBuffer[128]; // Buffer to hold incoming UDP packets
    //Serial.printf("temp1: %s\n", rxBuffer);
    while (true)
    {
        if(UDPManager::receive(rxBuffer, sizeof(rxBuffer)))
        {
            PacketParser::parse(rxBuffer); // Parse the received packet
        }
        vTaskDelay(pdMS_TO_TICKS(2)); // Delay for 10 milliseconds
    }
    
}
  