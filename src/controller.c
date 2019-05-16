//* PID controller for heli project

#include <stdint.h>
#include "controller.h"

// Function to update the control output depending on the current system errors and gains
int32_t controlUpdate(controller_t *system)
{
    int32_t control = (system->Kp * system->p_error) + (system->Kd * (*system->d_error)) + (system->Ki * system->i_error);
    return control;
}


// TODO Functions to update what i_error and d_error are?
// Should these be included in controlUpdate?
int32_t iErrorUpdate(controller_t *system)
{
    // adds the integral of the most recently written error in system->error
    // to system->i_error
    // Le: system->i_error += error * delta_t;
}

int32_t dErrorUpdate(controller_t *system)
{
    // Updates the differential error
    // Le: d_error = (error - error_previous)/delta_t;
}