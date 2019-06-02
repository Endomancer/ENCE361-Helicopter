// *******************************************************
//
// rotors.c
//
// Support altering PWM duty cycles of each rotor on the 
// helicopter.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include "rotors.h"
#include "pwm.h"
#include "config.h"

static pwm_t* rotorMain;
static pwm_t* rotorTail;

// Initialise main and tail rotors
void initRotors()
{
    rotorMain = initPWM(M0PWM7_PC5);
    rotorTail = initPWM(M1PWM5_PF1);

    setPWMFreq(rotorMain, PWM_FREQUENCY);
    setPWMFreq(rotorTail, PWM_FREQUENCY);

    setPWMDuty(rotorMain, 0);
    setPWMDuty(rotorTail, 0);
}

// Get main rotor duty cycle
uint16_t getMainRotorDuty()
{
    return getPWMDuty(rotorMain);
}

// Get tail rotor duty cycle
uint16_t getTailRotorDuty()
{
    return getPWMDuty(rotorTail);
}

// Set main rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setMainRotorSpeed(uint16_t duty)
{
    // Limit upper rotor speed
    if (duty > ROTOR_DUTY_LIMIT)
    {
        duty = ROTOR_DUTY_LIMIT;
    }

    setPWMDuty(rotorMain, duty);
}

// Set tail rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setTailRotorSpeed(uint16_t duty)
{
    // Limit upper rotor speed
    if (duty > ROTOR_DUTY_LIMIT)
    {
        duty = ROTOR_DUTY_LIMIT;
    }

    setPWMDuty(rotorTail, duty);
}