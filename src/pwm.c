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

// Initialise GPIO as a PWM output
static void initPWMGPIO(gpio_t* gpio)
{
    SysCtlPeripheralEnable(gpio->periph);
    GPIOPinConfigure(gpio->config);
    GPIOPinTypePWM(gpio->base, gpio->pin);
}

// Initialise the selected PWM output
pwm_t* initPWM(pwm_outputs_t output)
{
    // Select PWM output
    pwm_t* pwm = pwm_outputs[output];

    // Divide PWM clock
	SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    // Enable PWM peripheral
    SysCtlPeripheralEnable(pwm->periph);

    // Initialise GPIO
    initPWMGPIO(&pwm->gpio);

    // Configure PWM counting mode
	PWMGenConfigure(pwm->base, pwm->gen, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // Enable the PWM generator
    PWMGenEnable(pwm->base, pwm->gen);

    return pwm;
}

// Set the duty cycle of a PWM output
// Values should range from 0 to 100
void setPWMDuty(pwm_t* pwm, uint16_t duty)
{
    // Compute PWM width using the period
    pwm->width = pwm->period * duty / PERCENT;
    PWMPulseWidthSet(pwm->base, pwm->outnum, pwm->width);
}

// Set the frequency of a PWM output
void setPWMFreq(pwm_t* pwm, uint16_t frequency)
{
    // Compute PWM period in clock cycles
    pwm->period = SysCtlClockGet() / PWM_DIVIDER / frequency;
	PWMGenPeriodSet(pwm->base, pwm->gen, pwm->period);
}

// Enable the selected PWM output
void enablePWM(pwm_t* pwm)
{
    PWMOutputState(pwm->base, pwm->outbit, true);
}

// Disable the selected PWM output
void disablePWM(pwm_t* pwm)
{
    PWMOutputState(pwm->base, pwm->outbit, false);
}