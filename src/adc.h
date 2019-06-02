// *******************************************************
// adc.h
//
// Support for ADC usage on the Tiva TM4C123 Development Board.
// Specifically using channel 9 to read values from an analog sensor.
// 
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

extern uint32_t calibrationReference;

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void);

// Initialise the ADC
void initADC(void);

// Trigger ADC sampling process
void triggerADC(void);

// Return the average value in the circular buffer
uint32_t averageADCVal(void);

// Return the average ADC value relative to the calibrated reference
uint32_t getAltitude(void);

// Calculate height as a percentage
int32_t getHeight(void);

#endif
