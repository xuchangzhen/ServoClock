#include "stm32f10x.h"                  // Device header

void MyTIM_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef TimeBaseInitStructure;
			
	TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TimeBaseInitStructure);
	
	//TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM3, ENABLE);

}

