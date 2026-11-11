#ifndef __MYSPI_H
#define __MYSPI_H

#include "stm32f10x.h"                  // Device header
typedef enum{
	MYSPI_OK,
	MYSPI_ERROR_STOP_TXE_BUSY,
	MYSPI_ERROR_SWAPBYTE_TXE_BUSY,
	MYSPI_ERROR_SWAPBYTE_RECEIVE,
	
	
}MYSPI_STATUS;

void MySPI_Init(void);
void MySPI_Start(void);
MYSPI_STATUS MySPI_Stop(void);
MYSPI_STATUS MySPI_SwapByte(uint8_t SendData, uint8_t* ReceiveData);
MYSPI_STATUS MySPI_SendByte(uint8_t Data);
MYSPI_STATUS MySPI_ReceiceByte(uint8_t* Data);

#endif

