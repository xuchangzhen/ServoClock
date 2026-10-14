#include "ServoCalibration.h"
#include "W25Q64.h"
#include "Delay.h"
#include "stdlib.h"
#include "ServoRun.h"

//#define SERVO_CALIB_SIZE		6U
ServoCalibration_t ServoCaliBration[ServoCount_All];

//将舵机值统一设置
void ServoCalibration_Init(uint16_t PWM_NOW[], uint16_t Count ) {
	for (uint16_t i = 0; i < Count; ++i ) {
		ServoCaliBration[i] = (ServoCalibration_t)SERVO_PWM_DEFAULT;
		PWM_NOW[i] = ServoCaliBration[i].PWMOn;
	}
}

void ServoCalibration_On(void) {  //校准ON
	int16_t PWMChange;
	PWMChange = (int16_t)strtol(&USART2_Buffer[4], NULL, 10);
	ServoPWM[SerialRxData].PWMOn = PWMChange + ServoPWM[SerialRxData].PWMOn;
	ServoPWM[SerialRxData].PWMOn = Check_PWMValue(ServoPWM[SerialRxData].PWMOn);
	PCA9685_SETPWM(SerialRxData, 0, ServoPWM[SerialRxData].PWMOn);
}

void ServoCalibration_Off(void) {  //校准OFF
	int16_t PWMChange;
	PWMChange = (int16_t)strtol(&USART2_Buffer[4], NULL, 10);
	ServoPWM[SerialRxData].PWMOff = PWMChange + ServoPWM[SerialRxData].PWMOff;
	ServoPWM[SerialRxData].PWMOff = Check_PWMValue(ServoPWM[SerialRxData].PWMOff);
	PCA9685_SETPWM(SerialRxData, 0, ServoPWM[SerialRxData].PWMOff);
}

//保存PWM数据
uint8_t ServoCalibration_Save(ServoCalibration_t* ServoCalibration, uint16_t Count){
	uint32_t Length = (uint32_t)Count * sizeof(ServoCalibration[0]);
//	uint8_t Data[SERVO_CALIB_SIZE];
	
//    Data[0] = 'S';
//    Data[1] = 'C';
//    Data[2] = 'V';
//    Data[3] = '1';
	
//	Data[4] = (uint8_t)(PWMValue & 0xff);
//	Data[5] = (uint8_t)(PWMValue >> 8);
	W25Q64_STATUS Result;
	Result = W25Q64_WriteData(SERVO_CALIB_ADDRESS, (uint8_t*)ServoCalibration, Length);
	if (Result != W25Q64_OK) return 1;
	
	return 0;
	
}

//获取PWM数据，并检测该位数据是否正确
uint8_t ServoCalibration_Load(ServoCalibration_t* ServoCalibration, uint16_t Count, uint8_t* ERROR_POS) {
	uint32_t Length = (uint32_t)Count * sizeof(ServoCalibration[0]);
//	uint8_t Data[SERVO_CALIB_SIZE];
//	ServoCalibration_t Value;
	W25Q64_ReadData(SERVO_CALIB_ADDRESS, (uint8_t*)ServoCalibration, Length);
	
//    if (Data[0] != 'S' ||
//        Data[1] != 'C' ||
//        Data[2] != 'V' ||
//        Data[3] != '1') {
//        return 0;
//    }
		
//	Value = (
//		(uint16_t)Data[4] |
//		((uint16_t)Data[5] << 8)
//	);
	uint8_t cnt = 0;
	for (int i = 0; i < Count; ++i ){
		if (ServoCalibration[i].PWMOn < SERVO_PWM_MIN || ServoCalibration[i].PWMOn > SERVO_PWM_MAX) return 0;
		if (ServoCalibration[i].PWMOff < SERVO_PWM_MIN || ServoCalibration[i].PWMOff > SERVO_PWM_MAX) return 0;
		++cnt;
		*ERROR_POS = cnt;
	}
	
	return 1;
}

//检查PMW值是否符合要求
uint16_t Check_PWMValue(uint16_t PWMValue) {
	if (PWMValue < SERVO_PWM_MIN) return SERVO_PWM_MIN;
	if (PWMValue > SERVO_PWM_MAX) return SERVO_PWM_MAX;
	
	return PWMValue;
}

