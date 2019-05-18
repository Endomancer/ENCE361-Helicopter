#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

// TODO: Yuck... but we don't have a better place to put it currently
typedef enum
{
    LANDED,
    SWEEPING,
    FLYING,
    LANDING
} control_states_t;

// PID structure
typedef struct
{
    uint16_t Kp;        // Proportional gain
    uint16_t Ki;        // Integral gain
    uint16_t Kd;        // Derivative gain
    int32_t p_error;    // Proportional error
    int32_t i_error;    // Integral error
    int32_t d_error;    // Derivative error
    int32_t reference;  // Reference value
} pid_t;

// Initialise a PID controller instance
void initController(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd);

// Update the gains of a specified PID controller
// Used for gain scheduling and experimentally finding gains
void updateGains(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd);

// Update the controller output based on the current system error and gains
uint16_t controlUpdate(pid_t *pid, int32_t error, uint32_t dT);

#endif 
