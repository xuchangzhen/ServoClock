#include "MyADC.h"

volatile uint16_t ADC_DMA_Buffer[ADC_DMA_BUFFER_SIZE];

void MyADC_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //开启DMA时钟
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //将ADC时钟分频
	
	//初始化DMA1外设
//	DMA_InitTypeDef DMA_InitStructure;
//	DMA_DeInit(DMA1_Channel1);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DMA_Buffer;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据源是外设
//	DMA_InitStructure.DMA_BufferSize = ADC_DMA_BUFFER_SIZE;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //疑问
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);	
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;  //疑问
	ADC_ExternalTrigConvCmd(ADC1, DISABLE);  //允许外部触发
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);	
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	
}

//获取ADC初始值(旋转编码器值)
uint16_t MyADC_GetValue(void) {
	
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	return ADC_GetConversionValue(ADC1);
	
}

//软件滤波
uint16_t MyADC_GetAverageValue(void){
	uint32_t Sum = 0;
	uint8_t Size = sizeof(ADC_DMA_Buffer) / sizeof(ADC_DMA_Buffer[0]);
	for (uint8_t i = 0; i < Size; ++i ) {
		Sum += ADC_DMA_Buffer[i];
	}
	return Sum / Size;
}

//获取电压值
float ADC_GetVoltage(void) {
	uint16_t ADC_Value;
	
	ADC_Value = MyADC_GetValue();
	
	return (float)ADC_Value * 3.3f / 4095.0f;
	
}

//映射ADC的值到PWM
uint16_t Serve_ADCToPWM(uint16_t ADCValue){
	uint32_t PWMValue;
	
	PWMValue = 102U + (512U - 102U) * (uint32_t)ADCValue / 4095U;
	
	return (uint16_t)PWMValue;
}
