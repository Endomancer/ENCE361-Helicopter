#ifndef YAW_H
#define YAW_H

#include <stdint.h>
#include "pid.h"

// Initialse yaw controller
void initYaw();

// Update yaw controller
void updateYaw(uint32_t time);

// Change yaw controller mode
void changeYawMode(control_states_t newState);

// Increase yaw by 15 degrees (clockwise)
// Only used while flying
void increaseYaw();

// Decrease yaw by 15 degrees (counterclockwise)
// Only used while flying
void decreaseYaw();

#endif