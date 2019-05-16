//* PID controller for heli project

#include "controller.h"

#define BUF_SIZE 3
#define SCALING_FACTOR 100 // TODO
#define MAX_PWM 80

void initController(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

void proportional(pid_t* pid, int32_t pos)
{
    static int32_t prev_pos = 0;
    pid->p_error = pos - prev_pos;
    prev_pos = pos;
}

void integral(pid_t* pid)
{
    pid->i_error += pid->p_error;
}

void derivative(pid_t* pid)
{
    //static int32_t d_errors[BUF_SIZE] = {0};
    //static uint8_t index = 0;
    static int32_t prev_error = 0;

    // TODO: Average derivative
    pid->d_error = pid->p_error - prev_error;
    prev_error = pid->p_error;
}

uint16_t control_update(pid_t* pid, int32_t pos)
{
    proportional(pid, pos);
    integral(pid);
    derivative(pid);

    int32_t control = pid->Kp * pid->p_error
                    + pid->Ki * pid->i_error
                    + pid->Kd * pid->d_error;
    
    control /= SCALING_FACTOR;

    if (control > MAX_PWM)
    {
        control = MAX_PWM;
    }
    else if (control < 0)
    {
        control = 0;
    }
    
    return (uint32_t) control;
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