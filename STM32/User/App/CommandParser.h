#ifndef __COMMANDPARSER_H
#define __COMMANDPARSER_H

#include "stm32f10x.h"                  // Device header

typedef enum{  //舵机运行状态
	COMMANDPARSE_NONE,
	//COMMANDPARSE_SELECT_SERVO,
	COMMANDPARSE_ADDJUST_PWM,
	COMMANDPARSE_SAVE,
	COMMANDPARSE_TIME,
	COMMANDPARSE_START,
	COMMANDPARSE_STOP
} ParseCommand_t;

extern ParseCommand_t CommandMode; 			//命令状态
extern ParseCommand_t CommandBeforeJust;	//校准前舵机状态

void CommandParse_USART1(void);  //USART1  接收舵机校准编号
void CommandParse_USART2(void);  //USART2  接收舵机校准信号和时间数据

#endif
