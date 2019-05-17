//* PID controller for heli project

#include "controller.h"

#define BUF_SIZE 3
#define SCALING_FACTOR 100 // TODO
#define MAX_PWM 80

// Initialise a PID controller instance
pid_t initController(uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid_t pid;
    pid.Kp = Kp;
    pid.Ki = Ki;
    pid.Kd = Kd;
    pid.p_error = 0;
    pid.i_error = 0;
    pid.d_error = 0;
    return pid;
}

// Update the gains of a specified PID controller, to be used for
// gain scheduling and finding gains
void updateGains(pid_t* pid, uint16_t Kp, uint16_t Ki, uint16_t Kd)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

// Update the controller output
uint16_t control_update(pid_t* pid, int32_t pos, uint32_t dT, int32_t desired)
{
    pid->p_error = desired - pos;
    // I error and D error might not work due to integer math
    pid->i_error += pid->p_error*dT/1000;
    pid->d_error = (pid->p_error - pid->d_error)/(dT/1000);

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
}