#include "ServoClockApp.h"
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
#include "MyKey.h"
#include "CommandParser.h"
#include "string.h"
#include "USART_Test.h"


//时钟应用初始化
void ServoClockApp_Init(void) {
	MyI2C1_Init();
	PCA9685_SetFreq50Hz();
	MyUSART2_Init();
	MyADC_Init();
	MySPI_Init();
	MyKey_Init();
	MyUSART1_Init();
	NVIC_USART1_Init();
	MyTIM_Init();
	
	Servo_Init();
	LED_init();
	LED_Green_OFF();
	LED_Yellow_OFF();
	
	AppMode = SERVO_MODE_RUN;
}


void ServoClockApp_Process(void){  //程序进程
	if (AppMode == SERVO_MODE_RUN) 				    Servo_Run();   			 //运行态             //进入校准后会出现无法回到RUN状态，需要设置触发按键
	else if (AppMode == SERVO_MODE_CALIBRATE_ON) 	Servo_CalibrationOn();   //校准态             
	else if (AppMode == SERVO_MODE_CALIBRATE_OFF)   Servo_CalibrationOff();  //校准态
}    

void Servo_Run(void){  //舵机运行状态
	CommandParse_USART1();
	CommandParse_USART2();
	if (CommandMode == COMMANDPARSE_SELECT_SERVO ) AppMode = SERVO_MODE_CALIBRATE_ON;  //进入校准模式
	else if (CommandMode == COMMANDPARSE_TIME) {  //展示时间
		Show_HourAndMin();
		CommandMode = COMMANDPARSE_NONE;
	}
	else if (MyKey_GetPressed(GPIOB, GPIO_Pin_15) == 1) {  //舵机状态切换
		ServoToggle(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
	}
//			if (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET) {  //舵机定时转动
//				//ServoToggle(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
//				TIM_ClearFlag(TIM3, TIM_FLAG_Update);
//			}
} 				


void Servo_CalibrationOn(void) {  //网页校准ON数据
	MyUSART2_ReceiveBuff();
	if (CommandMode == COMMANDPARSE_ADDJUST_PWM) {  //获取到校准信号
		ServoCalibration_On();
	}else if (CommandMode == COMMANDPARSE_SAVE) {
		LED_Yellow_ON();
		AppMode = SERVO_MODE_CALIBRATE_OFF;
	}
	CommandMode = COMMANDPARSE_NONE;
}
	//ADC校准数据
//			ADC_Value = MyADC_GetValue();
//			ServoPWM[SerialRxData].PWMOn = Serve_ADCToPWM( ADC_Value );
//			Result = PCA9685_SETPWM(SerialRxData, 0, ServoPWM[SerialRxData].PWMOn);
	//按键再次按下保存数据


void Servo_CalibrationOff(void) {  //网页校准ON数据
	MyUSART2_ReceiveBuff();
	if (CommandMode == COMMANDPARSE_ADDJUST_PWM) {  //获取到校准信号
		ServoCalibration_Off();
	}else if (CommandMode == COMMANDPARSE_SAVE) {  //保存ON 和 OFF 数据
		memcpy(ServoCaliBration, ServoPWM, sizeof(ServoPWM));
		ServoCalibration_Save(ServoCaliBration, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
		LED_Green_OFF();
		LED_Yellow_OFF();
		AppMode = SERVO_MODE_RUN;
		SerialRxFlag = 0;
	}
	AppMode = SERVO_MODE_RUN;
	CommandMode = COMMANDPARSE_NONE;
}  
	










