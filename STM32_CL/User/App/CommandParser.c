#include "CommandParser.h"
#include "MyUSART.h"
#include "string.h"
#include "stdlib.h"
#include "ServoCalibration.h"
#include "LED_Show.h"
#include "ServoRun.h"
#include "MyKey.h"
#include "ServoClockApp.h"

ParseCommand_t CommandMode		 = COMMANDPARSE_TIME;  //初始状态

uint8_t USART2_CycleBuffer[USART2_CycleSize];          //指令接收循环缓冲区
uint8_t USART2_Cycle_WriteIndex = 0;     //循环缓冲区写入位
uint8_t USART2_Cycle_ReadIndex = 0;		//循环缓冲区读取位

uint8_t USART2_CommandBuffer[USART2_BufferSize];      //读取指令存放数组
uint32_t Command_Index = 0;                           //指令数组下标
uint8_t Command_Start = 0;

void CommandParse_USART1(void) {  //USART1  接收舵机校准编号
	if (SerialRxFlag == 1) {  //接收到校准信号
		char Buffer[3];
		char* End;
		strcpy(Buffer, USART1_Buffer);
		SerialRxData = strtol(Buffer, &End, 10);
		if (SerialRxData < ServoCount_All && 
			End != Buffer && *End == '\0'
		) {  //判断获取到的参数是否合法
		    LED_Green_ON();
			//CommandMode = COMMANDPARSE_ADJUST_PWM;
		}else SerialRxFlag = 0;
	}
	
	//解析到校准模式，esp32状态也要随之转换
}


/**
状态1: PWM校准信号
状态2: 时钟,日期数据
*/
void CommandParse_USART2(const char* CommandBuffer) {  //USART2
	//舵机校准状态
	if (AppMode == APP_MODE_CALIBRATE_ON || AppMode == APP_MODE_CALIBRATE_OFF) {  //舵机调整阶段 
		if (strncmp(CommandBuffer, "ADJUST:END", 10) == 0) {  //退出校准模式
			SerialRxFlag = 0;  //避免错误退出导致的接收错误
			CommandMode  = COMMANDPARSE_TIME;  
		}else if (strncmp(CommandBuffer, "PWM:", 4) == 0) {  //获取到的数据为PWM调整信号
			CommandMode = COMMANDPARSE_ADJUST_PWM;
		}else if ( (strncmp(CommandBuffer, "SAVE", 4) == 0) || (MyKey_GetPressed(GPIOB, GPIO_Pin_13) == 1) ) {  //获取到数据保存信号
			CommandMode = COMMANDPARSE_SAVE;
		}else {  //校准时期不允许其余指令到来
			return ;
		}
	}
	
	//解析RUN指令
	if (strncmp(CommandBuffer, "ADJUST:START", 12) == 0) {  //时钟校准
		CommandMode = COMMANDPARSE_ADJUST_PWM;
	}else if (strncmp(CommandBuffer, "TIME:", 5) == 0) {  //时间到来
		ClockTime_Valid(CommandBuffer);
	}else if (strncmp(CommandBuffer, "ON", 2) == 0) {  //时钟开启
		CommandMode = COMMANDPARSE_ON;
	}else if (strncmp(CommandBuffer, "OFF", 3) == 0) {  //时钟关闭
		CommandMode = COMMANDPARSE_OFF;
	}

	//解析显示指令
	if (strncmp(CommandBuffer, "YEAR", 4) == 0) {
		CommandMode = COMMANDPARSE_TIME;
		DisplayMode = DISPLAY_YEAR;
	}else if (strncmp(CommandBuffer, "MONTH", 5) == 0) {
		CommandMode = COMMANDPARSE_TIME;
		DisplayMode = DISPLAY_MONTH;
	}else if (strncmp(CommandBuffer, "HOUR:", 4) == 0) {
		CommandMode = COMMANDPARSE_TIME;
		DisplayMode = DISPLAY_HOUR;
	}else if (strncmp(CommandBuffer, "MINUTE", 6) == 0) {
		CommandMode = COMMANDPARSE_TIME;
		DisplayMode = DISPLAY_MINUTE;
	}
}


USART2_Cycle_Length_t USART2_CycleRemain_Get() {  //获取缓冲区剩余长度
    USART2_Cycle_Length_t Length = 0;
	if (USART2_Cycle_WriteIndex == USART2_Cycle_ReadIndex) {  //不存在未读取数据
        Length = USART2_CycleSize - 1;
	}else if (USART2_Cycle_WriteIndex > USART2_Cycle_ReadIndex) {  //写入位置在后
	    Length = USART2_CycleSize - USART2_Cycle_WriteIndex + USART2_Cycle_ReadIndex - 1;
	}else if (USART2_Cycle_ReadIndex - 1 > USART2_Cycle_WriteIndex) {  //写入已经回头
	    Length = USART2_Cycle_ReadIndex - USART2_Cycle_WriteIndex - 1;
	}
    return Length;
}

uint8_t USART2_Cycle_Empty(void) {  //判断缓冲区是否为空
    return USART2_CycleRemain_Get() == USART2_CycleSize - 1;
}

void USART2_Cycle_Write(uint8_t* Data, USART2_Cycle_Length_t Length) {  //将数据写入缓冲区
    USART2_Cycle_Length_t RemainLength = USART2_CycleRemain_Get();  //查询缓冲区剩余空间
    if (RemainLength < Length) return ;  //剩余空间不足
    else {
        if (USART2_CycleSize - USART2_Cycle_WriteIndex > Length) {  //后面剩余位置可以完全写入新数据
            memcpy(USART2_CycleBuffer + USART2_Cycle_WriteIndex, Data, Length);

        }else {  //需要分批写入
            USART2_Cycle_Length_t WriteLength = USART2_CycleSize - USART2_Cycle_WriteIndex;
            memcpy(USART2_CycleBuffer + USART2_Cycle_WriteIndex, Data, WriteLength);
            memcpy(USART2_CycleBuffer, Data + WriteLength, Length - WriteLength);

        }
        USART2_Cycle_WriteIndex = (USART2_Cycle_WriteIndex + Length) % USART2_CycleSize;  //下次写入位置
    }
}

void USART2_Cycle_Read(void) {  //将新指令读取到指令数组
    Command_Index %= (UINT32_MAX - 1);  //避免越界
    if (USART2_Cycle_Empty()) return ;  //缓冲区为空，不进行操作

    if (USART2_CycleBuffer[USART2_Cycle_ReadIndex] == '#') {
        Command_Index = 0;
        Command_Start = 1;
    }else if (USART2_CycleBuffer[USART2_Cycle_ReadIndex] == '@') {
        USART2_CommandBuffer[Command_Index] = '\0';
        if (Command_Start) CommandParse_USART2((char*)USART2_CommandBuffer);  //获取完整帧后立刻进行解析
        Command_Start = 0;
    }else {  //将数据写入到指令数组
        USART2_CommandBuffer[Command_Index] = USART2_CycleBuffer[USART2_Cycle_ReadIndex];
        Command_Index = (Command_Index + 1) % (USART2_BufferSize);
    }
    USART2_Cycle_ReadIndex = (USART2_Cycle_ReadIndex + 1) % USART2_CycleSize;
}
