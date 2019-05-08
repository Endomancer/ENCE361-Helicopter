#ifndef UART_H
#define UART_H

#include <stdint.h>

//********************************************************
// initUART - 8 bits, 1 stop bit, no parity
//********************************************************
void initUART(void);

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend(char *pucBuffer);

void UARTAltitude(int16_t altitude);

void UARTAngle(int16_t angle);

void UARTMainPWM();

void UARTTailPWM();

#endif
