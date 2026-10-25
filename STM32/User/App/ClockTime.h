#ifndef __CLOCKTIME_H
#define __CLOCKTIME_H

#include "stm32f10x.h"                  // Device header
#include "MyUSART.h"

//时间结构体
typedef struct {
	uint8_t   Second;
	uint8_t   Minite;
	uint8_t   Hour;
	uint16_t  Year;
} ClockTime_t;

//显示模式
typedef enum {
	DISPLAY_YEAR,
    DISPLAY_HOUR,
    DISPLAY_MINUTE
} DisplayMode_t;

extern uint8_t TimeValid;  //时间是否更新
extern DisplayMode_t DisplayMode;  //显示模式

uint8_t ClockTime_Parse(const char* Data);  //检验时间是否合法
uint8_t Get_MinAndSec(ClockTime_t* Time);   //获取分 秒
uint8_t Get_HourAndMin(ClockTime_t* Time);  //获取时 分
uint8_t Get_Year(ClockTime_t* Time);        //获取年份
void ClockTime_Valid(const char* Data);     //时间是否可用

#endif
