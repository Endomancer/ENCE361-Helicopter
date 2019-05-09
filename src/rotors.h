#ifndef ROTORS_H
#define ROTORS_H

#include "pwm.h"

extern pwm_t* rotorMain;
extern pwm_t* rotorTail;

void initRotors();

#endif