#ifndef __SERVOCLOCKAPP_H
#define __SERVOCLOCKAPP_H

#include "stm32f10x.h"                  // Device header

typedef enum {  //APP运行状态
	APP_MODE_NONE,
	APP_MODE_RUN,
	APP_MODE_OFF,
	APP_MODE_CALIBRATE_ON,
	APP_MODE_CALIBRATE_OFF,
	APP_MODE_ERROR
} ServoApp_Mode;

static ServoApp_Mode AppMode = APP_MODE_ERROR;

void ServoClockApp_Init(void);      //时钟应用初始化
void ServoClockApp_Process(void);   //程序进程
void Clock_Run(void); 				//舵机运行状态
void Clock_Off(void);				//时钟关闭
void Clock_CalibrationOn(void);		//舵机校准On
void Clock_CalibrationOff(void);	//舵机校准Off
void Clock_Count30(void);           //倒计时30s   (客服定制功能)

#endif 
