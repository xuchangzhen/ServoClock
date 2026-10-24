#include "stm32f10x.h"                  // Device header
#include "stdint.h"
#include "MyADC.h"
#include "MyUSART.h"
#include "LED_Show.h"
#include "ServoRun.h"
#include "ClockTime.h"
#include "stdio.h"
#include "Delay.h"
#include "MyTIM.h"
#include "MyI2C.h"
#include "PCA9685_L.h"
#include "PCA9685_R.h"
#include "PCA9685.h"
#include "MySPI.h"
#include "W25Q64.h"
#include "ServoCalibration.h"
#include "string.h"
#include "stdlib.h"
#include "USART_Test.h"
#include "ServoClockApp.h"


#define TEST_MODE 1



int main(void) {
#if TEST_MODE == 1
	MySPI_Init();
	MyI2C1_Init();
	MyUSART2_Init();
	PCA9685_SetFreq50Hz();
	Servo_Init();
	Servo_Off();
	//MyTIM2_Init();
	
	
	
	while (1) {
		//Servo_Count30();
	}
	

#else
	
	ServoClockApp_Init();  
	
	while (1) {
		ServoClockApp_Process();
	}
	
#endif
}	













//I2C先建立连接但是PCA9685未上电会导致通信失败，保留错误标志，需改进
//校准值未能成功获取问题/// 未获取位置如何处理
//重新插电给舵机，为什么会自动复位？
//取模方式互相校准



//单字符测试
//	MySPI_Init();
//	MyI2C1_Init();
//	MyUSART2_Init();
//	PCA9685_L_SetFreq50Hz();
//	Servo_Init();
//	
//	uint8_t Data;
//	
//	while (1) {
//		MyUSART2_ReceiveBuff();
//		
//		if (BufferFlag == 1) {
//			Data = USART2_Buffer[7];
//			ServoShowNum(Data);
//			BufferFlag = 0;
//		}
//	}
	
