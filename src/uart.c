#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "utils/uartstdio.h"
#include "rotors.h"

// USB Serial comms: UART0, Rx:PA0, Tx:PA1
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
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    
    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    UARTStdioConfig(0, BAUD_RATE, SysCtlClockGet());
}

//**********************************************************************
// Transmit the helicopter altitude via UART0
//**********************************************************************
void UARTAltitude(int16_t altitude, int32_t reference)
{
    UARTprintf("Altitude  = %4d [%d]\n", altitude, reference);
}

//**********************************************************************
// Transmit the helicopter angle via UART0
//**********************************************************************
void UARTAngle(int16_t angle, int32_t reference)
{
    UARTprintf("Angle     = %4d deg [%d]\n", angle, reference);
}

//**********************************************************************
// Transmit the helicopters main rotor duty cycle via UART0
//**********************************************************************
void UARTMainPWM()
{
    UARTprintf("Main Duty = %4d%%\n", getMainRotorDuty());
}

//**********************************************************************
// Transmit the helicopters tail rotor duty cycle via UART0
//**********************************************************************
void UARTTailPWM()
{
    UARTprintf("Tail Duty = %4d%%\n", getTailRotorDuty());
}

//**********************************************************************
// Transmit a calibrating message via UART0
//**********************************************************************
void UARTCalibrating(bool calibrating, bool initial)
{
    char calString[] = "------------------\nCalibrating ";
    char endString[] = "\n------------------\n";

    if (calibrating)
    {
        // Write initial calibrating message
        if (initial)
        {
            UARTprintf(calString);
        }
        UARTprintf(".");
    }
    else
    {
        // End the calibration message with a line
        UARTprintf(endString);
    }
}