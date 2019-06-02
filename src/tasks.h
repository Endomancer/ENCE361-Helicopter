// *******************************************************
//
// tasks.h
//
// Support for all tasks required for the project, these are:
//      - ADC
//      - Buttons
//      - Calibration
//      - Controller
//      - OLEDDisplay
//      - UART
//
// Edited for the helicopter project by Thu_am_group1
// Last modified:  2.6.19
//
// *******************************************************

#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "task.h"

void createTasks(void);

#endif