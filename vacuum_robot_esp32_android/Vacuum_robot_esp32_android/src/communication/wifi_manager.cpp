#include <Arduino.h>
#include "wifi_manager.h"
#include <WiFi.h>
#include "common/config.h"

namespace WiFiManager
{
    void connect()
    {
        const int MAX_RETRIES = 20; // Maximum number of connection attempts
        int retryCount = 0;

        Serial.printf("Connecting to %s...\n", Config::WIFI_SSID);
        WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);

        while(WiFi.status() != WL_CONNECTED && retryCount < MAX_RETRIES)
        {
            Serial.print(".");
            vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 500 milliseconds
            retryCount++;
        }

        if(WiFi.status() != WL_CONNECTED)
        {
            Serial.println("\nFailed to connect to WiFi.");
            return;
        }
        
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
} // namespace WiFiManager