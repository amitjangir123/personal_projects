#include <Arduino.h>
#include <HardwareSerial.h>

#define LED_PIN PC13

// Use USART1 (PA9=TX, PA10=RX) for serial communication
HardwareSerial Serial1(USART1);

void setup() {
  // Initialize Serial1 at 115200 baud
  Serial1.begin(115200);
  delay(2000);  // Wait for serial port to initialize
  
  Serial1.println("\n\n=== STM32F411CE LED Blink Demo ===");
  Serial1.println("Serial communication initialized!");
  Serial1.println("Board: STM32F411CE");
  
  pinMode(LED_PIN, OUTPUT);
  Serial1.println("GPIO configured");
  Serial1.println("Starting LED blink...\n");
}

void loop() {
  Serial1.println("LED ON");
  digitalWrite(LED_PIN, LOW);   // LED on
  delay(500);
  
  Serial1.println("LED OFF");
  digitalWrite(LED_PIN, HIGH);  // LED off
  delay(500);
  
  Serial1.print("Time: ");
  Serial1.print(millis());
  Serial1.println(" ms\n");
}