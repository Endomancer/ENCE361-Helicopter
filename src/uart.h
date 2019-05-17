#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
//********************************************************
// initUART - 8 bits, 1 stop bit, no parity
//********************************************************
void initUART(void);

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend(char *pucBuffer);

//**********************************************************************
// Transmit the helicopter altitude via UART0
//**********************************************************************
void UARTAltitude(int16_t altitude);

//**********************************************************************
// Transmit the helicopter angle via UART0
//**********************************************************************
void UARTAngle(int16_t angle);

//**********************************************************************
// Transmit the helicopters main rotor duty cycle via UART0
//**********************************************************************
void UARTMainPWM();

//**********************************************************************
// Transmit the helicopters tail rotor duty cycle via UART0
//**********************************************************************
void UARTTailPWM();

//**********************************************************************
// Transmit a calibrating message via UART0
//**********************************************************************
void UARTCalibrating(bool calibrating, bool initial);

#endif
