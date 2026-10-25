#include <Arduino.h>
#include <WiFiUdp.h>

#include "udp_manager.h"
#include "common/config.h"

namespace
{
    WiFiUDP udp;
}

namespace UDPManager
{
    bool begin()
    {
        if (!udp.begin(Config::UDP_PORT))
        {
            Serial.println("Failed to start UDP");
            return false;
        }
        Serial.printf("UDP started on port %u\n", Config::UDP_PORT);
        return true;
    }

    bool receive(char* buffer, size_t bufferSize)
    {
        int packetSize = udp.parsePacket();
        if (packetSize <= 0)
        {
            return false; // No packet received
        }

        //Serial.printf("Received UDP packet: %s\n", buffer);
        int length = udp.read(buffer, bufferSize - 1);
        if (length > 0)
        {
            buffer[length] = '\0'; // Null-terminate the string
            //Serial.printf("Received UDP packet: %s\n", buffer);
            return true;
        }
    }
} // namespace UDPManager