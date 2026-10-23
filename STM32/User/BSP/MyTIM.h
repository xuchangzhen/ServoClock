#ifndef __MYTIM_H
#define __MYTIM_H

#include "stm32f10x.h"                  // Device header

extern uint8_t Count_Mode;
extern uint8_t Count_Flag;

void MyTIM_Init(void);
void MyTIM2_Init(void);

#endif
