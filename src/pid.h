#ifndef CONTROLLER_H
#define CONTROLLER_H

// *******************************************************
//
// pid.h
//
// Support PID controllers with different types of input.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

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
} controller_t;

// Initialise a PID controller instance
void initPID(controller_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd);

// Update the gains of a specified PID controller
// Used for gain scheduling and experimentally finding gains
void updateGains(controller_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd);

// Update the controller output based on the current system error and gains
uint16_t updatePID(controller_t *pid, int32_t error, uint32_t dT, int32_t offset);

// Clamp input value between minimum and maximum thresholds
int32_t clamp(int32_t value, int32_t min, int32_t max);

// Ramp the PID reference towards the desired reference
void ramp(controller_t* pid, int32_t reference, int32_t increment);

#endif 
