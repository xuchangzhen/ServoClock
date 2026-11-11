#ifndef __PCA9685_H
#define __PCA9685_H

#include "stm32f10x.h"                  // Device header
#include "PCA9685_L.h"
#include "PCA9685_R.h"

void PCA9685_SetFreq50Hz(void);  //设置PWM频率
void PCA9685_SETPWM(uint8_t Channel, uint16_t OnCount, uint16_t OffCount);  //设置对应板子频率

#endif
