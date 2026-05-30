#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize motor pins
void motorInit(void);

// Step a motor by N steps
void stepMotor(int motorPin, int dirPin, int steps, int direction);

// Generate a single pulse
void pulseStep(int stepPin);

// Set direction helper (1 = forward, 0 = reverse)
void setDirectionPin(int dirPin, int direction);

// Pulse multiple step pins simultaneously. `mask` uses bit i => pin i.
// `pulse_us` controls the HIGH-time for the pulse.
void pulseMask(unsigned int mask, unsigned int pulse_us);

// Delay wrapper (microseconds)
void delay_us(unsigned int us);

#ifdef __cplusplus
}
#endif

#endif
