#ifndef ALTITUDE_H
#define ALTITUDE_H

#include <stdint.h>
#include "controller.h"

void initAltitude();

void updateAltitude(uint32_t deltaTime);

void changeMode(control_states_t newState);

void increaseAltitude();

void decreaseAltitude();

#endif