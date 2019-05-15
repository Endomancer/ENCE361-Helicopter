#include "rotors.h"
#include "pwm.h"
#include "config.h"

#define ROTOR_DUTY_LIMIT 90

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

// Set main rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setMainRotorSpeed(uint16_t duty)
{
    if (duty > ROTOR_DUTY_LIMIT)
    {
        duty = ROTOR_DUTY_LIMIT;
    }

    setPWMDuty(rotorMain, duty);
}

// Set tail rotor speed (between 0 and ROTOR_DUTY_LIMIT)
void setTailRotorSpeed(uint16_t duty)
{
    if (duty > ROTOR_DUTY_LIMIT)
    {
        duty = ROTOR_DUTY_LIMIT;
    }

    setPWMDuty(rotorTail, duty);
}

// Enable main rotor
void enableMainRotor()
{
    enablePWM(rotorMain);
}

// Enable tail rotor
void enableTailRotor()
{
    enablePWM(rotorTail);
}

// Disable main rotor
void disableMainRotor()
{
    disablePWM(rotorMain);
}

// Disable tail rotor
void disableTailRotor()
{
    disablePWM(rotorTail);
}