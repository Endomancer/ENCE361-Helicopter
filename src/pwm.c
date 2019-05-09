#include "pwm.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#define PWM_DIVIDER_CODE    SYSCTL_PWMDIV_8
#define PWM_DIVIDER         8
#define PERCENT             100

extern pwm_t* pwm_outputs[]; // Defined in pwmConfig.c

static void initPWMGPIO(gpio_t* gpio)
{
    SysCtlPeripheralEnable(gpio->periph);
    GPIOPinConfigure(gpio->config);
    GPIOPinTypePWM(gpio->base, gpio->pin);
}

pwm_t* initPWM(pwm_outputs_t output)
{
    pwm_t* pwm = pwm_outputs[output];

	SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    SysCtlPeripheralEnable(pwm->periph);

    initPWMGPIO(&pwm->gpio);

	PWMGenConfigure(pwm->base, pwm->gen, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMOutputState(pwm->base, pwm->outbit, true);

    return pwm;
}

void setPWMDuty(pwm_t* pwm, uint16_t duty)
{
    pwm->width = pwm->period * duty / PERCENT;

    PWMPulseWidthSet(pwm->base, pwm->outnum, pwm->width);
}

void setPWMFreq(pwm_t* pwm, uint16_t frequency)
{
    pwm->period = SysCtlClockGet() / PWM_DIVIDER / frequency;

	PWMGenPeriodSet(pwm->base, pwm->gen, pwm->period);
}

void enablePWM(pwm_t* pwm)
{
    PWMGenEnable(pwm->base, pwm->gen);
}

void disablePWM(pwm_t* pwm)
{
    PWMGenDisable(pwm->base, pwm->gen);
}