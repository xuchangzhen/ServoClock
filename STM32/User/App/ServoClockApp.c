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

static uint8_t AppOff_Flag = 0;
ServoApp_Mode AppMode = APP_MODE_ERROR;

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
	
	AppMode = APP_MODE_RUN;
}


void ServoClockApp_Process(void){  //程序进程
	if (AppMode == APP_MODE_RUN) 				    Clock_Run();   			 //运行态           
	else if (AppMode == APP_MODE_CALIBRATE_ON) 		Clock_CalibrationOn();   //校准态             
	else if (AppMode == APP_MODE_CALIBRATE_OFF)   	Clock_CalibrationOff();  //校准态
	else if (AppMode == APP_MODE_OFF)				Clock_Off();			 //关闭运行				
}    

void Clock_Run(void){  //舵机运行状态
	CommandParse_USART1();
	CommandParse_USART2();
	if (CommandMode == COMMANDPARSE_ADDJUST_PWM ) AppMode = APP_MODE_CALIBRATE_ON;  //进入校准模式
	else if (CommandMode == COMMANDPARSE_STOP) 	  Clock_Off();		  				//进入校准模式
	else if (TimeValid) {  															//展示时间
		TimeValid = 0;
		Show_Time();
	}
	
	CommandMode = COMMANDPARSE_NONE;  //避免校准阻塞
} 		

void Clock_Off(void) {  //关闭运行
	AppMode = APP_MODE_OFF; 
	CommandParse_USART2();
	if (AppOff_Flag == 0) {  //避免重复执行函数
		Servo_Off();
		AppOff_Flag = 1;
	}
	
	if (CommandMode == COMMANDPARSE_START) {  //重新处于开启状态
		AppOff_Flag = 0;
		AppMode = APP_MODE_RUN;
	}
}


void Clock_CalibrationOn(void) {  		//网页校准ON数据
	CommandParse_USART1();
	CommandParse_USART2();
	if (CommandMode == COMMANDPARSE_TIME) {  //退出校准模式
		
		AppMode = APP_MODE_RUN;  
		CommandMode = COMMANDPARSE_NONE;
		SerialRxFlag = 0;
		return ;
	}
	
	if (MyKey_GetPressed(GPIOB, GPIO_Pin_15) == 1) {  //舵机状态切换 主要用于检测舵机位置是否正确
		ServoToggle(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
	}
	
	if (SerialRxFlag != 1) {  //等待校准位到来 
		CommandMode = COMMANDPARSE_NONE;  //避免执行过期命令
		return ;  
	}   

	if (CommandMode == COMMANDPARSE_ADDJUST_PWM) {  //获取到校准信号
		ServoCalibration_On();
	}else if (CommandMode == COMMANDPARSE_SAVE) {
		LED_Yellow_ON();
		AppMode = APP_MODE_CALIBRATE_OFF;
	}else if (CommandMode == COMMANDPARSE_TIME) {  //读取到时间指令 恢复显示时间
		AppMode = APP_MODE_RUN;
	}

	
	CommandMode = COMMANDPARSE_NONE;
}
	//ADC校准数据
//			ADC_Value = MyADC_GetValue();
//			ServoPWM[SerialRxData].PWMOn = Serve_ADCToPWM( ADC_Value );
//			Result = PCA9685_SETPWM(SerialRxData, 0, ServoPWM[SerialRxData].PWMOn);
	//按键再次按下保存数据


void Clock_CalibrationOff(void) {  //网页校准ON数据
	CommandParse_USART2();
	if (CommandMode == COMMANDPARSE_ADDJUST_PWM) {  //获取到校准信号
		ServoCalibration_Off();
	}else if (CommandMode == COMMANDPARSE_SAVE) {  //保存ON 和 OFF 数据
		memcpy(ServoCaliBration, ServoPWM, sizeof(ServoPWM));
		ServoCalibration_Save(ServoCaliBration, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
		LED_Green_OFF();
		LED_Yellow_OFF();
		AppMode = APP_MODE_CALIBRATE_ON;
		SerialRxFlag = 0;
	}
	CommandMode = COMMANDPARSE_NONE;
}  
       
	










