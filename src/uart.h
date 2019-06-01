#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

//********************************************************
// initUART - 8 bits, 1 stop bit, no parity
//********************************************************
void initUART(void);

//**********************************************************************
// Transmit the helicopter altitude via UART0
//**********************************************************************
void UARTAltitude(int16_t altitude, int32_t reference);

//**********************************************************************
// Transmit the helicopter angle via UART0
//**********************************************************************
void UARTAngle(int16_t angle, int32_t reference);

//**********************************************************************
// Transmit the helicopters main rotor duty cycle via UART0
//**********************************************************************
void UARTMainPWM(uint16_t duty);

//**********************************************************************
// Transmit the helicopters tail rotor duty cycle via UART0
//**********************************************************************
void UARTTailPWM(uint16_t duty);

//**********************************************************************
// Transmit a calibrating message via UART0
//**********************************************************************
void UARTCalibrating(bool calibrating, bool initial);

#endif
