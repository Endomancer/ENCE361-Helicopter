#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

/* controller struct */
typedef struct
{
	uint16_t Kp;   // Proportional gain
	uint16_t Ki; // Integral Gain
	uint16_t Kd; // Differential gain
    int32_t p_error; // Proportional error
    int32_t i_error; // Integral error
    int32_t *d_error; // Differential error
} controller_t;



#endif 