#ifndef __MYKEY_H
#define __MYKEY_H

#include "stm32f10x.h"                  // Device header

void MyKey_Init(void);  //按键初始化
uint8_t MyKey_GetPressed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  //检测按键按下

#endif
