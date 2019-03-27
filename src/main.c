#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"

#include "adc.h"
#include "buttons4.h"
#include "circBufT.h"
#include "clock.h"
#include "display.h"
#include "uart.h"

#define MAX_HEIGHT 993 // Corresponds to ~0.8 V
#define NUM_STATES 3

enum displayState
{
    ALTITUDE,
    MEAN_ADC,
    OFF
};

// Calibration:
// Record the mean ADC value at 4 different intervals and
// return the average of these values
int32_t
calibrate()
{
    int32_t sum = 0;
    clearDisplay();
    OLEDStringDraw("Calibrating", 0, 1);
    UARTSend("Calibrating");
    SysCtlDelay(SysCtlClockGet() / 8);

    sum += average();
    OLEDStringDraw(".", 11, 1);
    UARTSend(".");
    SysCtlDelay(SysCtlClockGet() / 8);

    sum += average();
    OLEDStringDraw(".", 12, 1);
    UARTSend(".");
    SysCtlDelay(SysCtlClockGet() / 8);

    sum += average();
    OLEDStringDraw(".", 13, 1);
    UARTSend(".\r\n");
    SysCtlDelay(SysCtlClockGet() / 8);

    sum += average();
    clearDisplay();

    return sum / 4;
}

int main(void)
{
    int16_t ADCValue, height;
    int32_t reference;
    enum displayState disp = ALTITUDE;
    char uartString[17];

    // Initialise peripherals
    initClock();
    initADC();
    initButtons();
    initUART();
    initSysTick();
    initDisplay();

    // Enable interrupts
    IntMasterEnable();

    // Initial calibration
    reference = calibrate();

    while (1)
    {
        // Calibrate when the left button is pressed
        if (checkButton(LEFT) == PUSHED)
        {
            reference = calibrate();
        }

        // Cycle through display states
        // ALTITUDE -> MEAN_ADC -> OFF
        if (checkButton(UP) == PUSHED)
        {
            disp = (disp + 1) % NUM_STATES;
            clearDisplay();
        }

        // Store average ADC value
        ADCValue = average();

        // Calculate the height as a percentage
        height = (reference - ADCValue) * 100 / MAX_HEIGHT;

        // Saturate the height: 0% -> 100%
        if (height < 0)
        {
            height = 0;
        }
        else if (height > 100)
        {
            height = 100;
        }

        // Display appropriate data on the OLED
        switch (disp)
        {
        case ALTITUDE:
            displayAltitude(height);
            break;

        case MEAN_ADC:
            displayMeanVal(ADCValue);
            break;

        case OFF:
            break;
        }

        // Send height and ADC data over UART
        usprintf(uartString, "Height = %3d%% | Mean ADC = %d\r\n", height, ADCValue);
        UARTSend(uartString);

        // Update at ~10 Hz
        SysCtlDelay(SysCtlClockGet() / 30);
    }
}
