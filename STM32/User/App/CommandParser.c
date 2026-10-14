#include "CommandParser.h"
#include "MyUSART.h"
#include "string.h"
#include "stdlib.h"
#include "ServoCalibration.h"
#include "LED_Show.h"
#include "ServoRun.h"
#include "MyKey.h"

ParseCommand_t CommandMode = COMMANDPARSE_NONE;

void CommandParse_USART1(void) {  //USART1  接收舵机校准编号
	if (SerialRxFlag == 1) {  //接收到校准信号
		char Buffer[3];
		char* End;
		strcpy(Buffer, USART1_Buffer);
		SerialRxData = strtol(Buffer, &End, 10);
		if (SerialRxData < ServoCount_All && 
			End != Buffer && *End == '\0'
		) {  //判断获取到的参数是否合法
			CommandMode = COMMANDPARSE_SELECT_SERVO;
			LED_Green_ON();
		}else SerialRxFlag = 0;
	}
	
	//解析到校准模式，esp32状态也要随之转换
}


/**
状态1: PWM校准信号
状态2: 时钟,日期数据
*/
void CommandParse_USART2(void) {  //USART2 
	MyUSART2_ReceiveBuff();
	if (BufferFlag != 1) return ;
	if (strncmp(USART2_Buffer, "PWM:", 4) == 0) {  //获取到的数据为PWM调整信号
		CommandMode = COMMANDPARSE_ADDJUST_PWM;
	}else if ( (strncmp(USART2_Buffer, "Save", 4) == 0) || (MyKey_GetPressed(GPIOB, GPIO_Pin_13) == 1) ) {  //获取到数据保存信号
		CommandMode = COMMANDPARSE_SAVE;
	}else CommandMode = COMMANDPARSE_TIME;
	
	BufferFlag = 0;
}
