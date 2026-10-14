#ifndef __MYUSART_H
#define __MYUSART_H

#include "stm32f10x.h"                  // Device header

#define USART1_BufferSize 	3U
#define USART2_BufferSize 	16U

//校准中断
extern uint8_t SerialRxFlag;
extern uint8_t SerialRxData;
extern char USART1_Buffer[USART1_BufferSize];

//时间数组
extern char USART2_Buffer[USART2_BufferSize];
static uint8_t BufferIndex = 0;
extern uint8_t BufferFlag;


void MyUSART1_Init(void);
void MyUSART2_Init(void);
void NVIC_USART1_Init(void);
void MyUSART1_SendChar(char c);
void MyUSART1_SendStr(char* str);
void MyUSART2_ReceiveBuff(void);

#endif
