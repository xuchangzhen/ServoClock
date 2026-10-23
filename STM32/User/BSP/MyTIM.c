#include "stm32f10x.h"                  // Device header

uint8_t Count_Mode  = 1;
uint8_t Count_Flag  = 0;

void MyTIM_Init(void){  //ADC定时采样
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

void MyTIM2_Init(void) {  //倒计时
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TimeBaseInitStructure;
			
	TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInitStructure.TIM_Period = 10000 - 1;
	TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TimeBaseInitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void){
	if (Count_Mode == 0) {  //不需要计时
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		return ;
	}
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		Count_Flag = 1;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}





