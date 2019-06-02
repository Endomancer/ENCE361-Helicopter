// *******************************************************
//
// clock.h
//
// Support for using the system clock on the
//  Tiva processor.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_RATE_HZ 256

// Required by FreeRTOS
extern uint32_t SystemCoreClock;

// Initialise system clock
void initClock(void);

#endif