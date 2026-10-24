#include "ClockTime.h"
#include "string.h"

//判断位数是否为数字
static uint8_t is_digit(uint8_t c) {
	if (c >= '0' && c <= '9') return 1;
	return 0;
}

//校验时钟数据
uint8_t ClockTime_Parse(const char* Data, ClockTime_t* Time) {
	uint8_t Second, Minite, Hour;
	uint16_t Year;
	
	if (Data == 0 || Time == 0) return 0;
	
	//检查位
	if (!is_digit(Data[0])  ||
		!is_digit(Data[1])  ||
		!is_digit(Data[3])  ||
		!is_digit(Data[4])  || 
		!is_digit(Data[6])  ||
		!is_digit(Data[7])  || 
		!is_digit(Data[9])  || 
		!is_digit(Data[10]) ||
		!is_digit(Data[11]) || 
		!is_digit(Data[12]) 
	) return 0;
	
	if (Data[2] != ':' || Data[5] != ':' || Data[8] != '/') return 0;
	
	Hour   = (Data[0] - '0') * 10 + Data[1] - '0';
	Minite = (Data[3] - '0') * 10 + Data[4] - '0';
	Second = (Data[6] - '0') * 10 + Data[7] - '0';
	Year   = (Data[9] - '0') * 1000 + (Data[10] - '0') * 100 + 
			 (Data[11] - '0') * 10 + Data[12] - '0';
	
	if (Second > 60 || Minite > 60 || Hour > 24) return 0;
	
	Time->Year   = Year;
	Time->Hour 	 = Hour;
	Time->Minite = Minite;
	Time->Second = Second;
	
	return 1;
}

//返回 分秒
uint8_t Get_MinAndSec(ClockTime_t* Time) {
	ClockTime_t Tmp;
	char* TimeData = strchr(USART2_Buffer, ':');
	if ( ClockTime_Parse(TimeData + 1, &Tmp) == 0) return 0;
	Time->Minite = Tmp.Minite;
	Time->Second = Tmp.Second;
	return 1;
}	

//返回 时分
uint8_t Get_HourAndMin(ClockTime_t* Time) {
	ClockTime_t Tmp;
	char* TimeData = strchr(USART2_Buffer, ':');
	if ( ClockTime_Parse(TimeData + 1, &Tmp) == 0) return 0;
	Time->Hour = Tmp.Hour;
	Time->Minite = Tmp.Minite;
	return 1;
}

//获取年份
uint8_t Get_Year(ClockTime_t* Time) {
	ClockTime_t Tmp;
	char* TimeData = strchr(USART2_Buffer, ':');
	if ( ClockTime_Parse(TimeData + 1, &Tmp) == 0) return 0;
	Time->Year = Tmp.Year;
	return 1;
}
