#ifndef __MYUART_H
#define __MYUART_H

#define STM32_UART UART_NUM_2
#define STM32_UART_TX_PIN 17
#define STM32_UART_RX_PIN 16

#include <stdint.h>

void UART2_Init(void);
void UART2_SendData(char* Data, uint8_t Len);

#endif