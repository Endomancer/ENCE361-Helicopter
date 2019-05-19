//* PID controller for heli project

#include "controller.h"
#include "config.h"

#define BUF_SIZE 3
#define SCALING_FACTOR 100 // TODO
#define MAX_PWM 98

// Initialise a PID controller instance
void initController(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->p_error = 0;
    pid->i_error = 0;
    pid->d_error = 0;
    pid->reference = 0;
}

// Update the gains of a specified PID controller
// Used for gain scheduling and experimentally finding gains
void updateGains(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

// Update the controller output based on the current system error and gains 
uint16_t controlUpdate(pid_t* pid, int32_t error, uint32_t dT, int32_t offset)
{
    pid->p_error = error;
    pid->i_error += pid->p_error * dT / CPU_CLOCK_SPEED;
    pid->d_error = (pid->p_error - pid->d_error) * CPU_CLOCK_SPEED / dT;

    if (pid->i_error > 500)
        pid->i_error = 500;

    int32_t control = pid->Kp * pid->p_error
                    + pid->Ki * pid->i_error
                    + pid->Kd * pid->d_error
                    + SCALING_FACTOR * offset;
    
    control /= SCALING_FACTOR;

    if (control > MAX_PWM)
    {
        control = MAX_PWM;
    }
    else if (control < 2)
    {
        control = 2;
    }
    
    return control;
}