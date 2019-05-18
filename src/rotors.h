#ifndef ROTORS_H
#define ROTORS_H

#include <stdint.h>

// Initialise main and tail rotors
void initRotors();

// Get main rotor duty cycle
uint16_t getMainRotorDuty();

// Get tail rotor duty cycle
uint16_t getTailRotorDuty();

// Set main rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setMainRotorSpeed(uint16_t duty);

// Set tail rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setTailRotorSpeed(uint16_t duty);

#endif