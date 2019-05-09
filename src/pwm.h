#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "pwmConfig.h"

typedef struct
{
    uint32_t periph;
    uint32_t config;
    uint32_t base;
    uint32_t pin;
} gpio_t;

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

pwm_t* initPWM(pwm_outputs_t output);

void setPWMDuty(pwm_t* pwm, uint16_t duty);

void setPWMFreq(pwm_t* pwm, uint16_t frequency);

void enablePWM(pwm_t* pwm);

void disablePWM(pwm_t* pwm);

#endif