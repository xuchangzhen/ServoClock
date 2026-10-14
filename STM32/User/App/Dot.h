#ifndef __DOT_H
#define __DOT_H

#include "stm32f10x.h"                  // Device header


#define   DOT_COUNT        2U

void Dot_On(void);  	 //冒号开启
void Dot_Off(void);  	 //冒号关闭
void Dot_Toggle(void);   //冒号翻转


#endif
