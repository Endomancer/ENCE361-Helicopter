#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"

#define SAMPLE_RATE_HZ 256

// Required by FreeRTOS
extern uint32_t SystemCoreClock;

//********************************************************
// Initialisation functions: clock, SysTick, display, UART
//********************************************************
void initClock(void);

//*******************************************************************
void initSysTick(void);

//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void);

#endif
