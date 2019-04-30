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