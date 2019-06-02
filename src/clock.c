// *******************************************************
//
// clock.c
//
// Support for using the system clock on the
//  Tiva processor.
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#include "clock.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

// Required by FreeRTOS
uint32_t SystemCoreClock;

// Initialise system clock
void initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Configure FreeRTOS clock speed
	SystemCoreClock = SysCtlClockGet();
}