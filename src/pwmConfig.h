#ifndef PWMCONFIG_H
#define PWMCONFIG_H

// *******************************************************
//
// pwmConfig.h
//
// Configuration required for all possible PWM channels avaiale
// on the Tiva board.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

// Available PWM outputs
typedef enum pwm_outputs_t
{
	M0PWM0_PB6,
	M0PWM1_PB7,
	M0PWM2_PB4,
	M0PWM3_PB5,
	M0PWM4_PE4,
	M0PWM5_PE5,
	M0PWM6_PC4,
	M0PWM6_PD0,
	M0PWM7_PC5,
	M0PWM7_PD1,
	M1PWM0_PD0,
	M1PWM1_PD1,
	M1PWM2_PA6,
	M1PWM2_PE4,
	M1PWM3_PA7,
	M1PWM3_PE5,
	M1PWM4_PF0,
	M1PWM5_PF1,
	M1PWM6_PF2,
	M1PWM7_PF3
} pwm_outputs_t;

#endif