#include "Arduino.h"
#include "backlash.h"
#include "bed_leveling.h"
#include "config.h"
#include "gcode_executor.h"
#include "gcode_parser.h"
#include "kinematics.h"
#include "motor_control.h"
#include "motion.h"
#include "motion_driver.h"

HardwareSerial Serial1(USART1); // Use USART1 (PA9=TX, PA10=RX) for serial communication

// Simple static state for demo
static Position curPos = {0.0f, 0.0f, 0.0f};
static MotorSteps curSteps = {0,0,0};
static DirectionState ds = {1,1,1};
static int absolute_mode = 1;

// Simple bridge functions to forward motor_control APIs into executor/driver
static void hw_pulse_stub(int pin) {
  pulseStep(pin);
}

static void hw_setdir_stub(int pin, int dir) {
  setDirectionPin(pin, dir);
}

static void hw_delay_stub(unsigned int us) {
  delay_us(us);
}

// Process a single G-code line: parse and execute. Keeps static state across calls.
static void processGcodeLine(const char* line) {
  if (!line) return;

  //Serial1.println("[STM32]: main.cpp: processGcodeLine");
  GcodeCommand cmd = parseGcode(line);

  //Serial1.print("[STM32]: Parsed command: ");
  //Serial1.print(cmd.command);
  //Serial1.print(cmd.code);

  
  executeGcodeCommand(&cmd, &curPos, &curSteps, &ds, &absolute_mode,
                      hw_pulse_stub, hw_setdir_stub, hw_delay_stub);
}

void setup() {
  Serial1.begin(9600);
  Serial1.println("[STM32]: setup start");
  motorInit();
  Serial1.println("[STM32]: setup complete");

  delay(2000);  // Wait for serial port to initialize

  Serial1.println("READY");
}



void loop() {

  if (Serial1.available() > 0) {
    String line = Serial1.readStringUntil('\n');
    Serial1.println("[STM32]: Received line:");
    //Serial1.println(line);
    processGcodeLine(line.c_str());
    //delay(2000);
    Serial1.println("READY");
  }
  

  
 
  
  //processGcodeLine(entry);
  //delay(10); // small delay between processing entries
        
}
