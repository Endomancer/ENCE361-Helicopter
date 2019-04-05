#ifndef QUAD_H
#define QUAD_H

#include <stdint.h>

// Initialise the quadrotor port/pins, and interrupt handler
void initQaud(void);

// Reset the value of the quadrotor slit counter variable to zero
void resetQuad(void);

// Converts the quadrotor slit counter to a value in degrees (of rotation of the helicopter)
uint32_t getQuadAngle(void);

// Iterates the quadrotor slit counter on an event, positively for CW, negatively for CCW
void QuadIntHandler(void);

#endif