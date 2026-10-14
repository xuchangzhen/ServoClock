#include "ClockTime.h"

//判断位数是否为数字
static uint8_t is_digit(uint8_t c) {
	if (c >= '0' && c <= '9') return 1;
	return 0;
}

//校验时钟数据
uint8_t ClockTime_Parse(const char* Data, ClockTime_t* Time) {
	uint8_t Second, Minite, Hour;
	
	if (Data == 0 || Time == 0) return 0;
	
	//检查位
	if (!is_digit(Data[0]) ||
		!is_digit(Data[1]) ||
		!is_digit(Data[3]) ||
		!is_digit(Data[4]) || 
		!is_digit(Data[6]) ||
		!is_digit(Data[7])
	) return 0;
	
	if (Data[2] != ':' || Data[5] != ':') return 0;
		
	Hour = (Data[0] - '0') * 10 + Data[1] - '0';
	Minite = (Data[3] - '0') * 10 + Data[4] - '0';
	Second = (Data[6] - '0') * 10 + Data[7] - '0';
	
	if (Second > 60 || Minite > 60 || Hour > 24) return 0;
	
	Time->Hour 	 = Hour;
	Time->Minite = Minite;
	Time->Second = Second;
	
	return 1;
}

//返回 分秒
uint8_t Get_MinAndSec(ClockTime_t* Time) {
	ClockTime_t Tmp;
	if ( ClockTime_Parse(USART2_Buffer, &Tmp) == 0) return 0;
	Time->Minite = Tmp.Minite;
	Time->Second = Tmp.Second;
	return 1;
}

//返回 时分
uint8_t Get_HourAndMin(ClockTime_t* Time) {
	ClockTime_t Tmp;
		if ( ClockTime_Parse(USART2_Buffer, &Tmp) == 0) return 0;
		Time->Hour = Tmp.Hour;
		Time->Minite = Tmp.Minite;
		BufferFlag = 0; 
	return 1;
}
