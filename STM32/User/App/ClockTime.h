#ifndef __CLOCKTIME_H
#define __CLOCKTIME_H

#include "stm32f10x.h"                  // Device header
#include "MyUSART.h"


typedef struct {
	uint8_t Second;
	uint8_t Minite;
	uint8_t Hour;
} ClockTime_t;

uint8_t ClockTime_Parse(const char* Data, ClockTime_t* Time);  //检验时间是否合法
uint8_t Get_MinAndSec(ClockTime_t* Time);  //获取分 秒
uint8_t Get_HourAndMin(ClockTime_t* Time);  //获取时 分

#endif
