#include "clock.h"
#include "adc.h"
#include "buttons4.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"

// Required by FreeRTOS
uint32_t SystemCoreClock;

//********************************************************
// Initialisation functions: clock, SysTick, display, UART
//********************************************************
void initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

	// Configure FreeRTOS clock speed 
	SystemCoreClock = SysCtlClockGet();
}

//*******************************************************************
void initSysTick(void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    //ADCProcessorTrigger(ADC0_BASE, 3);
    //g_ulSampCnt++;

    // Poll buttons
    updateButtons();
}