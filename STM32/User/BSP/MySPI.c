#include "MySPI.h"

#define MYSPI_TIMEOUT 100000UL
#define MYSPI_NSS_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define MYSPI_NSS_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_4);

void MySPI_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	MYSPI_NSS_HIGH();
	SPI_I2S_DeInit(SPI1);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
	
	SPI_Cmd(SPI1, ENABLE);
}

void MySPI_Start(void) {
	MYSPI_NSS_LOW();
}

MYSPI_STATUS MySPI_Stop(void) {
	
	uint32_t Timeout = MYSPI_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (Timeout-- == 0) {
			MYSPI_NSS_HIGH();
			return MYSPI_ERROR_STOP_TXE_BUSY;
		}
	}

	MYSPI_NSS_HIGH();
	return MYSPI_OK;
}

MYSPI_STATUS MySPI_SwapByte(uint8_t SendData, uint8_t* ReceiveData) {

	uint32_t Timeout = MYSPI_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (Timeout-- == 0) {
			MYSPI_NSS_HIGH();
			return MYSPI_ERROR_SWAPBYTE_TXE_BUSY;
		}
	}
	SPI_I2S_SendData(SPI1, SendData);
	
	Timeout = MYSPI_TIMEOUT;
	//等待数据进入接收寄存器
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
		if (--Timeout == 0) {
			MYSPI_NSS_HIGH();
			return MYSPI_ERROR_SWAPBYTE_RECEIVE;
		}
	}
	
	*ReceiveData = SPI_I2S_ReceiveData(SPI1);
	
	return MYSPI_OK;
}

//发送数据 1 BYTE
MYSPI_STATUS MySPI_SendByte(uint8_t Data){
	uint8_t Temp;
	MYSPI_STATUS Result;
	Result = MySPI_SwapByte(Data, &Temp);
	
	return Result;
}

//接收数据
MYSPI_STATUS MySPI_ReceiceByte(uint8_t* Data){
	uint8_t Temp = 0xff;
	MYSPI_STATUS Result;
	Result = MySPI_SwapByte(Temp, Data);
	
	return Result;
}

