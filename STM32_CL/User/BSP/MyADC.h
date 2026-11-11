#ifndef __MYADC_H
#define __MYADC_H

#include "stm32f10x.h"                  // Device header

#define ADC_DMA_BUFFER_SIZE 16

extern volatile uint16_t ADC_DMA_Buffer[ADC_DMA_BUFFER_SIZE];

void MyADC_Init(void);
uint16_t MyADC_GetValue(void);
uint16_t MyADC_GetAverageValue(void);
float ADC_GetVoltage(void);
uint16_t Serve_ADCToPWM(uint16_t ADCValue);


#endif
