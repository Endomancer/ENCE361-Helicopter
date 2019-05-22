#ifndef ALTITUDE_H
#define ALTITUDE_H

#include <stdint.h>
#include "pid.h"

// Initialise controller
void initController();

// Update controller
void updateController(uint32_t deltaTime);

// Change controller mode
void changeMode(control_states_t newState);

// Increase altitude by 10%
// Only used while flying
void increaseAltitude();

// Decrease altitude by 10%
// Only used while flying
void decreaseAltitude();

// Increase yaw by 15 degrees (clockwise)
// Only used while flying
void increaseYaw();

// Decrease yaw by 15 degrees (counterclockwise)
// Only used while flying
void decreaseYaw();

int32_t getAltitudeReference();

int32_t getYawReference();

control_states_t getState();

#endif