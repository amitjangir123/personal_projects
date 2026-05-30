/*
#include <Arduino.h>
#include <HardwareSerial.h>

#define LED_PIN PC13

// Use USART1 (PA9=TX, PA10=RX) for serial communication
HardwareSerial serial(USART1);

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);   // LED on
    delay(500);
    digitalWrite(LED_PIN, HIGH);  // LED off
    delay(500);
  }
}

void setup() {
  // Initialize Serial1 at 9600 baud
  serial.begin(9600);
  delay(2000);  // Wait for serial port to initialize
  
  serial.println("\n\n=== STM32F411CE LED Control ===");
  serial.println("Type 'yes' to blink LED or 'no' to skip");
  serial.println("Waiting for input...\n");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off by default
}

String read_serial_until_newline() {
  char incomingstring[32];
  int index = 0;
  while (true) {
    if (serial.available() > 0) {
      char incomingByte = serial.read() & 0x7F;  // AND with 0x7F to remove the first bit
      //serial.println(incomingByte);  // Echo the character back to the serial monitor
      if (incomingByte == '\n') {
        break;  // End of line
      }
      incomingstring[index++] = incomingByte;
    }
  }
  incomingstring[index] = '\0';  // Null-terminate the string
  return String(incomingstring);
}

void loop() {
  //blinkLED(3);  // Blink once to indicate we're alive

  //delay(1000);  // Wait for a moment before reading serial input

  serial.println("READY");  // Signal to the monitor that we're ready to receive data

  //delay(1000);  // Wait for the monitor to process the ready signal
  serial.println(read_serial_until_newline());  // Read and print the incoming data
}
*/