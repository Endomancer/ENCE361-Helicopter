#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

// PID structure
typedef struct
{
    uint16_t Kp;        // Proportional gain
    uint16_t Ki;        // Integral gain
    uint16_t Kd;        // Derivative gain
    int32_t p_error;    // Proportional error
    int32_t i_error;    // Integral error
    int32_t d_error;    // Derivative error
} pid_t;

void initController(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd);

void controller(pid_t* pid);

// *******************************************************
// controlUpdate: Updates the control output based on the 
// current system error and gains 
int32_t controlUpdate(controller_t *system);


#endif 
