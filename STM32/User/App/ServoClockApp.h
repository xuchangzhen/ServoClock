#ifndef __SERVOCLOCKAPP_H
#define __SERVOCLOCKAPP_H

#include "stm32f10x.h"                  // Device header

typedef enum {  //APP运行状态
	SERVO_MODE_RUN,
	SERVO_MODE_CALIBRATE_ON,
	SERVO_MODE_CALIBRATE_OFF,
	SERVO_MODE_ERROR
} ServoApp_Mode;

static ServoApp_Mode AppMode = SERVO_MODE_ERROR;

void ServoClockApp_Init(void);  //时钟应用初始化
void ServoClockApp_Process(void);   //程序进程
void Servo_Run(void); 				//舵机运行状态
void Servo_CalibrationOn(void);		//舵机校准On
void Servo_CalibrationOff(void);	//舵机校准Off

#endif 
