#ifndef CONFIG_H
#define CONFIG_H

namespace Config
{
    // Wi-Fi configuration
    constexpr char WIFI_SSID[] = "Airtel_Jangir_2G";
    constexpr char WIFI_PASSWORD[] = "12341234";

    // UDP configuration
    constexpr uint16_t UDP_PORT = 5000; // Port for UDP communication

    // Built-in LED Pin
    //constexpr uint8_t BUILT_IN_LED_PIN = 2; // GPIO pin for built-in LED (usually GPIO 2 on ESP32)

    // Bumper Sensor Pin
    constexpr uint8_t BUMPER_PIN = 14; // GPIO pin for bumper sensor

    // IR Sensor Pin
    constexpr uint8_t IR_SENSOR_PIN = 22; // GPIO pin for IR sensor

    // Charging Sensor Pin
    constexpr uint8_t CHARGING_SENSOR_PIN = 23; // GPIO pin for charging

    // UART configuration for Serial2
    constexpr uint32_t UART_BAUD_RATE = 115200; // Baud rate for Serial2 communication
    constexpr uint8_t UART_RX_PIN = 16; // GPIO pin for UART RX
    constexpr uint8_t UART_TX_PIN = 17; // GPIO pin for UART TX

    // Max speed
    constexpr float MAX_SPEED = 5000.0f; // Maximum speed for the motors

} // namespace Config


#endif // CONFIG_H