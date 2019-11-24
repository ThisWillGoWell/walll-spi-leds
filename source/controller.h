/*
 * uart.h
 *
 *  Created on: Aug 21, 2019
 *      Author: will
 */

#ifndef UART_H_
#define UART_H

#define UART UART1
#define UART_CLKSRC SYS_CLK
#define UART_CLK_FREQ CLOCK_GetFreq(SYS_CLK)
#define UART_IRQn UART1_RX_TX_IRQn
#define UART_IRQHandler UART1_RX_TX_IRQHandler

#define UART_BAUD 9600

#define NUM_MODES 3

void initController();
uint8_t getMode();
void sendUart(uint8_t * send, int length );

#endif /* UART_H_ */
