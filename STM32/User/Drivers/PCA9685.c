#include "PCA9685.h"



//设置PWM频率
void PCA9685_SetFreq50Hz(void) {
	PCA9685_L_SetFreq50Hz();
	PCA9685_R_SetFreq50Hz();
}

//设置对应板子频率
void PCA9685_SETPWM(uint8_t Channel, uint16_t OnCount, uint16_t OffCount) {
	uint32_t Address;
	//根据通道选择地址
	if (Channel <= 13 || Channel == 28)  Address = PCA9685_L_ADDRESS;  //冒号上位置 以及 左边时钟块
	else {
						Address = PCA9685_R_ADDRESS;
						Channel = Channel == 29 ? Channel - 15 : Channel - 14;  //把传入参数设置为板子R上的对应值
	}			   
	
	Channel = Channel == 28 ? 14 : Channel;  //映射冒号上位置
	
	if (Address == PCA9685_L_ADDRESS) 			PCA9685_L_SETPWM(Channel, OnCount, OffCount);
	else if (Address == PCA9685_R_ADDRESS)		PCA9685_R_SETPWM(Channel, OnCount, OffCount);	

}
