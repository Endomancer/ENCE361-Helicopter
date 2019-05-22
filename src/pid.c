//* PID controller for heli project

#include "pid.h"
#include "config.h"
#include "FreeRTOS.h"

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
    pid->d_error = (error - pid->p_error) * configTICK_RATE_HZ / dT;
    pid->p_error = error;
    pid->i_error += pid->p_error * dT / configTICK_RATE_HZ;
    pid->i_error = clamp(pid->i_error, -5000, 5000);
    
    int32_t control = pid->Kp * pid->p_error
                    + pid->Ki * pid->i_error / SCALING_FACTOR
                    + pid->Kd * pid->d_error / SCALING_FACTOR
                    + offset;
    
    control /= SCALING_FACTOR;
    control = clamp(control, MIN_PWM, MAX_PWM);
    
    return control;
}

// Clamp input value between minimum and maximum thresholds
int32_t clamp(int32_t value, int32_t min, int32_t max)
{
    if (value > max)
    {
        value = max;
    }
    else if (value < min)
    {
        value = min;
    }
    
    return value;
}

// Ramp the PID reference towards the desired reference
void ramp(controller_t* pid, int32_t reference, int32_t increment)
{
    int32_t difference = reference - pid->reference;

    if (difference > increment)
    {
        pid->reference += increment;
    }
    else if (-difference > increment)
    {
        pid->reference -= increment;
    }
    else
    {
        pid->reference = reference;
    }
}