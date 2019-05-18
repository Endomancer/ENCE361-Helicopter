#ifndef YAW_H
#define YAW_H

#include <stdint.h>
#include "controller.h"

void initYaw();

void updateYaw(uint32_t deltaTime);

void changeYawMode(control_states_t newState);

void increaseYaw();

void decreaseYaw();

#endif