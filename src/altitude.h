#ifndef ALTITUDE_H
#define ALTITUDE_H

#include <stdint.h>
#include "controller.h"

// Initialise altitude controller
void initAltitude();

// Update altitude controller
void updateAltitude(uint32_t deltaTime);

// Change controller mode
void changeMode(control_states_t newState);

// Increase altitude by 10%
// Only used while flying
void increaseAltitude();

// Decrease altitude by 10%
// Only used while flying
void decreaseAltitude();

// Return the altitude reference
int32_t getAltitudeReference();

#endif