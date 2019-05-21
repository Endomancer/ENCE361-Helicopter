//* PID controller for heli project

#include "pid.h"
#include "config.h"

#define BUF_SIZE 3
#define MAX_PWM 98
#define MIN_PWM 2

// Initialise a PID controller instance
void initPID(controller_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
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
void updateGains(controller_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

// Update the controller output based on the current system error and gains 
uint16_t updatePID(controller_t* pid, int32_t error, uint32_t dT, int32_t offset)
{
    pid->d_error = (error - pid->p_error) * CPU_CLOCK_SPEED / dT;
    pid->p_error = error;
    pid->i_error += pid->p_error * dT / CPU_CLOCK_SPEED;

    if (pid->i_error > 500)
        pid->i_error = 500;
    else if (pid->i_error < -500)
        pid->i_error = -500;

    int32_t control = pid->Kp * pid->p_error
                    + pid->Ki * pid->i_error / SCALING_FACTOR
                    + pid->Kd * pid->d_error / SCALING_FACTOR
                    + offset;
    
    control /= SCALING_FACTOR;

    if (control > MAX_PWM)
    {
        control = MAX_PWM;
    }
    else if (control < MIN_PWM)
    {
        control = MIN_PWM;
    }
    
    return control;
}