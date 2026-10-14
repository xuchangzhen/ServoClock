#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f10x.h"                  // Device header

typedef enum{
	MYI2C_OK,
	MYI2C_ERROR_BUSY,
	MYI2C_ERROR_START,
	MYI2C_ERROR_SEND_SLAVEADDRESS,
	MYI2C_ERROR_SEND_RESADDRESS,
	MYI2C_ERROR_SEND_DATA,
	MYI2C_ERROR_STOP,
	MYI2C_ERROR_RESTART,
	MYI2C_ERROR_SEND_RECEIVE,
	MYI2C_ERROR_RECEIVE
} MYI2C_STATUS;

void MyI2C1_Init(void);
MYI2C_STATUS MyI2C_WriteRegister(I2C_TypeDef* I2Cx, uint32_t SlaveAddress7Bit, uint32_t RegisterAddress, uint8_t Data);
MYI2C_STATUS MyI2C_ReadRegister(I2C_TypeDef* I2Cx, uint32_t SlaveAddress7Bit, uint32_t RegisterAddress, uint8_t* Data);

#endif
