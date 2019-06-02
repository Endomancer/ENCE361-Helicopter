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

// Create FreeRTOS tasks
void createTasks();

// Start the FreeRTOS task scheduler
void startScheduler();

#endif