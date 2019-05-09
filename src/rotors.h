#ifndef ROTORS_H
#define ROTORS_H

#include <stdint.h>

// Initialise main and tail rotors
void initRotors();

// Set main rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setMainRotorSpeed(uint16_t duty);

// Set tail rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setTailRotorSpeed(uint16_t duty);

// Enable main rotor
void enableMainRotor();

// Enable tail rotor
void enableTailRotor();

// Disable main rotor
void disableMainRotor();

// Disable tail rotor
void disableTailRotor();

#endif