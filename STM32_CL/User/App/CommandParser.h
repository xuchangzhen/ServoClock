#ifndef __COMMANDPARSER_H
#define __COMMANDPARSER_H

#include "MyUSART.h"
#include "stm32f10x.h"                  // Device header

#define USART2_Cycle_Length_t		uint8_t
#define USART2_CycleSize            128U

typedef enum{  //舵机运行状态
	COMMANDPARSE_NONE,
	//COMMANDPARSE_SELECT_SERVO,
	COMMANDPARSE_ADJUST_PWM,
	COMMANDPARSE_SAVE,
	COMMANDPARSE_TIME,
	COMMANDPARSE_ON,
	COMMANDPARSE_OFF
} ParseCommand_t;

extern uint8_t USART2_CommandBuffer[USART2_BufferSize];      //读取指令存放数组

extern ParseCommand_t CommandMode; 			//命令状态
extern ParseCommand_t CommandBeforeJust;	//校准前舵机状态

void CommandParse_USART1(void);  //USART1  接收舵机校准编号
void CommandParse_USART2(const char* CommandBuffer);  //USART2  接收舵机校准信号和时间数据
void USART2_Cycle_Write(uint8_t* Data, USART2_Cycle_Length_t Length);  //向缓冲区写入数据
void USART2_Cycle_Read(void);                                          //读出缓冲区数据

#endif
