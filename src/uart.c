#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "utils/ustdlib.h"
#include "config.h"
#include "rotors.h"
#include "pwm.h"

#define MAX_STR_LEN 16
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE UART0_BASE
#define UART_USB_PERIPH_UART SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX GPIO_PIN_1
#define UART_USB_GPIO_PINS UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// initUART - 8 bits, 1 stop bit, no parity
//********************************************************
void initUART(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend(char *pucBuffer)
{
    // Loop while there are more characters to send.
    while (*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

//**********************************************************************
// Transmit the helicopter altitude via UART0
//**********************************************************************
void UARTAltitude(int16_t altitude, int32_t reference)
{
    char string[26]; // The string only needs 18 characters
    usnprintf(string, sizeof(string), "Altitude  = %4d [%d]\n", altitude, ((reference+1)*PERCENT/MAX_HEIGHT));
    UARTSend(string);
}

//**********************************************************************
// Transmit the helicopter angle via UART0
//**********************************************************************
void UARTAngle(int16_t angle, int32_t reference)
{
    char string[28]; // The string only 22 characters
    int32_t roundedReference;
    if (reference >= 0)
    {
        roundedReference = reference + 1;
    }
    else
    {
        roundedReference = reference - 1;
    }
    
    usnprintf(string, sizeof(string), "Angle     = %4ddeg [%d]\n", angle, (roundedReference*DEGREES/ROT_COUNT));
    UARTSend(string);
}

//**********************************************************************
// Transmit a calibrating message via UART0
//**********************************************************************
void UARTCalibrating(bool calibrating, bool initial)
{
    char calString[] = "----------------\nCalibrating ";
    char endString[] = "\n----------------\n";

    if (calibrating)
    {
        // Write initial calibrating message
        if (initial)
        {
            UARTSend(calString);
        }
        UARTSend(".");
    }
    else
    {
        // End the calibration message with a line
        UARTSend(endString);
    }
}

//**********************************************************************
// Transmit the helicopters main rotor duty cycle via UART0
//**********************************************************************
void UARTMainPWM()
{
    char string[20];                    // String is at max 20 characters long
    uint16_t duty = getMainRotorDuty(); // Get the duty cycle of the main rotor
    usnprintf(string, sizeof(string), "Main Duty = %4d%%\n", duty);
    UARTSend(string); // Send to UART
}

//**********************************************************************
// Transmit the helicopters tail rotor duty cycle via UART0
//**********************************************************************
void UARTTailPWM()
{
    char string[20];                    // String is at max 20 characters long
    uint16_t duty = getTailRotorDuty(); // Get the duty cycle of the tail rotor
    usnprintf(string, sizeof(string), "Tail Duty = %4d%%\n", duty);
    UARTSend(string); // Send to UART
}