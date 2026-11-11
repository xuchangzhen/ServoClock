#ifndef __SERVO_CALIBRATION_H
#define __SERVO_CALIBRATION_H

#include "stm32f10x.h"                  // Device header
//#include "stdbool.h"

#define SERVO_CALIB_ADDRESS		0x10000UL
#define ServoCount_All 			30U
#define ServoCount_Clock 		28U
#define ServoCount_Board		14U
#define ServoCount_Digit		7U

#define SERVO_PWM_MIN			102U
#define SERVO_PWM_MAX			512U
#define SERVO_PWM_DEFAULT		{321U, 321U} //避免相撞，直接放在中间位置     //{102U, 307U}

//typedef enum {  //校准程序运行状态
//    SERVO_MODE_RUN = 0,
//    SERVO_MODE_CALIBRATE_ON,
//	SERVO_MODE_CALIBRATE_OFF
//} ServoMode;

typedef struct{  //舵机状态
	uint16_t PWMOn;
	uint16_t PWMOff;
} ServoCalibration_t;

extern ServoCalibration_t ServoCaliBration[ServoCount_All];  //保存舵机校准值
void ServoCalibration_Init(uint16_t PWM_NOW[], uint16_t Count );  //舵机校准值初始化
void ServoCalibration_On(void);   //校准ON
void ServoCalibration_Off(void);  //校准OFF
uint8_t ServoCalibration_Save(ServoCalibration_t* ServoCalibration, uint16_t Count);  //保存校准值
uint8_t ServoCalibration_Load(ServoCalibration_t* ServoCalibration, uint16_t Count, uint8_t* CNT);  //加载校准值
uint16_t Check_PWMValue(uint16_t PWMValue);  //查看PWM值是否符合要求

#endif
