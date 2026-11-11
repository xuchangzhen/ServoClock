#include "MyKey.h"
#include "Delay.h"

//按键初始化
void MyKey_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

//按键按下检测
uint8_t MyKey_GetPressed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == SET) {
		Delay_ms(20);  //按下消抖
		
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == SET) {
			while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == SET);
			Delay_ms(20);  //松开消抖
			
			return 1;
		}
	}
	return 0;
}