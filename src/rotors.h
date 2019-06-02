#ifndef ROTORS_H
#define ROTORS_H

// *******************************************************
//
// rotors.c
//
// Support altering PWM duty cycles of each rotor on the 
// helicopter.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

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