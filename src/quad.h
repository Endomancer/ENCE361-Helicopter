#ifndef QUAD_H
#define QUAD_H

#include <stdint.h>

void initQaud(void);

void resetQuad(void);

uint32_t getQuadAngle(void);

void PortBIntHandler(void);

#endif