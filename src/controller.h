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

pid_t initController(uint16_t Kp, uint16_t Ki, uint16_t Kd);


// *******************************************************
// controlUpdate: Updates the control output based on the 
// current system error and gains 
uint16_t control_update(pid_t *pid, int32_t pos, uint32_t dT, int32_t desired, int32_t offset);


#endif 
