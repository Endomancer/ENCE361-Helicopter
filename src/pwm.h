#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "pwmConfig.h"

// PWM GPIO data
typedef struct
{
    uint32_t periph;
    uint32_t config;
    uint32_t base;
    uint32_t pin;
} gpio_t;

// PWM data
typedef struct
{
    uint32_t periph;
    uint32_t base;
    uint32_t gen;
    uint32_t outnum;
    uint32_t outbit;
    gpio_t gpio;
    uint32_t period;
    uint32_t width;
} pwm_t;

// Initialise the selected PWM output
pwm_t* initPWM(pwm_outputs_t output);

// Set the duty cycle of a PWM output
// Values should range from 0 to 100
void setPWMDuty(pwm_t* pwm, uint16_t duty);

// Set the frequency of a PWM output
void setPWMFreq(pwm_t* pwm, uint16_t frequency);

// Enable the selected PWM output
void enablePWM(pwm_t* pwm);

// Disable the selected PWM output
void disablePWM(pwm_t* pwm);

#endif