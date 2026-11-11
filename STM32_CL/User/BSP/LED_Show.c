#include "LED_Show.h"

//初始化LED
void LED_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStrucure;
	GPIO_InitStrucure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStrucure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrucure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStrucure);
	
	GPIO_InitStrucure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStrucure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrucure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStrucure);
}

//LED指示
void LED_Green_ON(void) {
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED_Green_OFF(void) {
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_Yellow_ON(void) {
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void LED_Yellow_OFF(void) {
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
}
