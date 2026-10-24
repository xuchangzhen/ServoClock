#ifndef __SERVO_RUN
#define __SERVO_RUN

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "ServoCalibration.h"
#include "PCA9685.h"
#include "ClockTIme.h"

#define Digit		4

// bit0~bit6 分别对应 a~g，1 表示显示，0 表示隐藏
const static uint8_t SegmentCode[10] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

extern ServoCalibration_t ServoPWM[ServoCount_All];  			//舵机临时保存值
extern uint16_t PWM_NOW[ServoCount_All];   						//舵机当前值

void Servo_Init(void);  										//舵机初始化
void ServoToggle(uint16_t PWM_NOW[], uint16_t Count); 			//依次反转舵机值
void ServoToggle_D(uint16_t PWM_NOW[], uint16_t Count );  		//双PCA9685反转舵机值
void ServoToggle_Quad(uint16_t PWM_NOW[], uint16_t Count );    	//四位同时反转 同段(数码管段)
void Servo_Off(void);   										//关闭状态
void ServoShowNum(uint8_t Num, uint8_t Digit_POS);              //根据断码表展示数据
void Show_MinAndSec(void);                                      //展示 分 秒
void Show_HourAndMin(void);  									//展示 时 分
void Show_Year(void);  											//展示 年份     没写
	
#endif
