#include "motor_control.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdio.h>
#include <unistd.h>

#ifndef HIGH
#define HIGH 1
#define LOW 0
#endif
#ifndef OUTPUT
#define OUTPUT 1
#endif

static inline void digitalWrite(int pin, int val) { (void)pin; (void)val; }
static inline void pinMode(int pin, int mode) { (void)pin; (void)mode; }
static inline void delayMicroseconds(unsigned int us) { if (us > 0) usleep((useconds_t)us); }
static inline void delay(unsigned int ms) { if (ms > 0) usleep((useconds_t)ms * 1000); }
#endif

/**
 * motor_control.c - Stepper motor control via GPIO bit-banging
 * 
 * This module generates step and direction signals for stepper motor drivers.
 * Uses simple GPIO toggling with microsecond delays (no timers/interrupts).
 * 
 * Stepper Motor Basics:
 * - Each HIGH→LOW pulse on STEP pin moves motor one step
 * - DIRECTION pin (HIGH/LOW) determines rotation direction
 * - Multiple pulses create smooth motion (micropulses can subdivide steps)
 * 
 * Example Timing (typical stepper driver):
 *   Setup time:  0.5 μs (before step pulse)
 *   Step pulse:  1-2 μs (HIGH)
 *   Hold time:   0.5 μs (after step pulse)
 *   Minimum between steps: ~1-5 μs (determines max speed)
 */

/**
 * pulseStep - Generate a single step pulse
 * 
 * Creates a pulse on the STEP pin:
 *   1. Set pin HIGH
 *   2. Hold for 2 microseconds
 *   3. Set pin LOW
 * 
 * Timing:
 *   HIGH duration: ~2 μs (stepper driver typical requirement)
 *   This is fast enough for normal CNC speeds
 * 
 * @param stepPin: GPIO pin number for STEP signal
 */
void pulseStep(int stepPin) {
  // Set STEP pin HIGH
  // This signals the motor driver to execute one step
  digitalWrite(stepPin, HIGH);
  
  // Wait for pulse width (stepper driver requirement)
  // 2 microseconds is typical for most stepper drivers
  // (enough time for optocoupler or direct logic to register)
  delayMicroseconds(2);
  
  // Set STEP pin LOW
  // Completes the pulse. Motor driver captures on rising edge or falling edge
  // depending on configuration (usually rising edge = step increment)
  digitalWrite(stepPin, LOW);
}

/**
 * stepMotor - Move a motor by N steps in specified direction
 * 
 * Generates multiple step pulses to move motor a given distance.
 * 
 * Motor Operation:
 *   1. Set direction (HIGH = forward, LOW = backward)
 *   2. Wait for direction setup time
 *   3. Generate N step pulses
 *   4. Each pulse = one microstepped increment
 * 
 * Example - Move 800 steps forward:
 *   stepMotor(MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN, 800, 1)
 *   → Sets DIR=HIGH
 *   → Generates 800 step pulses
 *   → Motor rotates 800 steps forward
 * 
 * Speed Control:
 *   For fastest speed: just pulse (1-2 μs between pulses)
 *   For slower speed: add delay between pulses
 *   Current implementation: ~5 μs per step = ~200 kHz pulse rate
 * 
 * @param stepPin:      GPIO pin for STEP signal
 * @param dirPin:       GPIO pin for DIRECTION signal
 * @param steps:        Number of steps to move (positive = move, negative = stay)
 * @param direction:    1 for forward, 0 for backward (or -1 for backward)
 */
void stepMotor(int stepPin, int dirPin, int steps, int direction) {
  // Handle negative step counts (ignore them, don't move)
  if (steps <= 0) {
    return;
  }
  
  // Set direction pin based on direction parameter
  // direction = 1 (or positive): set HIGH (forward)
  // direction = 0 or negative: set LOW (backward)
  if (direction > 0) {
    digitalWrite(dirPin, HIGH);    // Forward direction
  } else {
    digitalWrite(dirPin, LOW);     // Backward direction
  }
  
  // Wait for stepper driver to register direction change
  // Most drivers need 0.5-1.0 μs setup time before first pulse
  // Using 5 μs to be safe
  delayMicroseconds(5);
  
  // Generate step pulses
  // Each iteration = one step pulse
  for (int i = 0; i < steps; i++) {
    pulseStep(stepPin);
    
    // Minimum delay between pulses
    // This determines maximum speed
    // 5 μs = ~200 kHz pulse rate (reasonable for bit-banging)
    // Can reduce to 1 μs for faster speeds if driver supports it
    delayMicroseconds(5);
  }
}

/**
 * motorInit - Initialize all motor GPIO pins
 * 
 * Sets up the direction and step pins for all three motors (A, B, Z).
 * Should be called once during setup() before any motor movement.
 * 
 * Pin Configuration (from config.h):
 *   Motor A: PA0 (STEP), PA1 (DIR)
 *   Motor B: PA2 (STEP), PA3 (DIR)
 *   Motor Z: PA4 (STEP), PA5 (DIR)
 * 
 * GPIO Setup:
 *   - All pins set to OUTPUT mode
 *   - All pins set to LOW initially (no movement, backward direction)
 * 
 * Example Usage in main():
 *   void setup() {
 *     motorInit();  // Initialize all motor pins
 *     // Now ready to call stepMotor()
 *   }
 */
void motorInit(void) {
  // Configure Motor A pins
  pinMode(MOTOR_A_STEP_PIN, OUTPUT);
  pinMode(MOTOR_A_DIR_PIN, OUTPUT);
  digitalWrite(MOTOR_A_STEP_PIN, LOW);
  digitalWrite(MOTOR_A_DIR_PIN, LOW);
  // Configure Motor A enable (active LOW) and keep enabled
  pinMode(MOTOR_A_EN_PIN, OUTPUT);
  digitalWrite(MOTOR_A_EN_PIN, LOW);
  
  // Configure Motor B pins
  pinMode(MOTOR_B_STEP_PIN, OUTPUT);
  pinMode(MOTOR_B_DIR_PIN, OUTPUT);
  digitalWrite(MOTOR_B_STEP_PIN, LOW);
  digitalWrite(MOTOR_B_DIR_PIN, LOW);
  // Configure Motor B enable (active LOW) and keep enabled
  pinMode(MOTOR_B_EN_PIN, OUTPUT);
  digitalWrite(MOTOR_B_EN_PIN, LOW);
  
  // Configure Motor Z pins
  pinMode(MOTOR_Z_STEP_PIN, OUTPUT);
  pinMode(MOTOR_Z_DIR_PIN, OUTPUT);
  digitalWrite(MOTOR_Z_STEP_PIN, LOW);
  digitalWrite(MOTOR_Z_DIR_PIN, LOW);
  // Configure Motor Z enable (active LOW) and keep enabled
  pinMode(MOTOR_Z_EN_PIN, OUTPUT);
  digitalWrite(MOTOR_Z_EN_PIN, LOW);
}

void setDirectionPin(int dirPin, int direction) {
  if (direction > 0) digitalWrite(dirPin, HIGH);
  else digitalWrite(dirPin, LOW);
}

void pulseMask(unsigned int mask, unsigned int pulse_us) {
  // Set all selected step pins HIGH
  for (int pin = 0; pin < 32; pin++) {
    if (mask & (1u << pin)) {
      digitalWrite(pin, HIGH);
    }
  }
  // Hold pulse
  if (pulse_us > 0) delayMicroseconds(pulse_us);
  // Set all selected step pins LOW
  for (int pin = 0; pin < 32; pin++) {
    if (mask & (1u << pin)) {
      digitalWrite(pin, LOW);
    }
  }
}

void delay_us(unsigned int us) {
  delayMicroseconds(us);
}

/**
 * Performance Notes:
 * 
 * Current Timing per Step:
 *   - pulseStep() overhead: ~5-10 μs total
 *   - delayMicroseconds(5): ~5 μs
 *   - Total per step: ~10-15 μs
 *   - Maximum speed: ~67-100 kHz pulse rate
 * 
 * For faster speeds, you could:
 *   1. Reduce delayMicroseconds(5) to delayMicroseconds(1)
 *   2. Use inline assembly (advanced)
 *   3. Use hardware timers with interrupts (but you said no!)
 * 
 * For slower speeds:
 *   1. Add more delayMicroseconds() between pulses
 *   2. Add larger delays between motor operations
 *   3. Implements feedrate control naturally
 * 
 * Accuracy:
 *   - Every pulse = exactly 1 microstep
 *   - No stepping error accumulation
 *   - Perfect for simple non-servo systems
 */
