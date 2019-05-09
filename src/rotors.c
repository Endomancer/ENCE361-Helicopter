#include "rotors.h"
#include "pwm.h"
#include "config.h"

pwm_t* rotorMain;
pwm_t* rotorTail;

void initRotors()
{
    rotorMain = initPWM(M0PWM7_PC5);
    rotorTail = initPWM(M1PWM5_PF1);

    setPWMFreq(rotorMain, PWM_FREQUENCY);
    setPWMFreq(rotorTail, PWM_FREQUENCY);

    setPWMDuty(rotorMain, 0);
    setPWMDuty(rotorTail, 0);
}