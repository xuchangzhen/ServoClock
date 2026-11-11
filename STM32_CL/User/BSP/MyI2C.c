#include "MyI2C.h"

#define MYI2C_TIMEOUT 100000UL

//是不是应该检查从机地址是不是正确的，这个应该怎么检查？

void MyI2C1_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	I2C_DeInit(I2C1);  //先恢复状态，避免错误状态残留
	//初始化GPIOB为I2C要求
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//初始化I2C
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;  //设置占空比有什么用
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C1, &I2C_InitStructure);
	
	I2C_Cmd(I2C1, ENABLE);

} 

//清除错误标志位并结束通信
void MyI2C_ClearFlagAndStop(I2C_TypeDef* I2Cx) {
	if (I2C_GetFlagStatus(I2Cx ,I2C_FLAG_AF) == SET) I2C_ClearFlag(I2Cx ,I2C_FLAG_AF);
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

//向通信设备寄存器写入数据
MYI2C_STATUS MyI2C_WriteRegister(I2C_TypeDef* I2Cx, uint32_t SlaveAddress7Bit, uint32_t RegisterAddress, uint8_t Data){
	uint32_t TimeOut = MYI2C_TIMEOUT;
	
	//检查总线是否空闲
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_BUSY;
		}
	}
	
	//start, 进入EV5
	TimeOut = MYI2C_TIMEOUT;
	I2C_GenerateSTART(I2Cx, ENABLE);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_START;
		}
	}
	
	//发送从机地址，选择通信设备, 进入EV6
	TimeOut = MYI2C_TIMEOUT;
	I2C_Send7bitAddress(I2Cx, SlaveAddress7Bit << 1, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_SLAVEADDRESS;
		}
	}
	
	//选择内部寄存器，进入EV8_1，==存在疑问==,是需要一直等待发送完毕吗，移位寄存器空之后还需要检查EV8_2吗
	TimeOut = MYI2C_TIMEOUT;
	I2C_SendData(I2Cx, RegisterAddress);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR) {  //等待数据全部移除移位寄存器，即数据真正的发出去了
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_RESADDRESS;
		}
	}
	
	//向内部寄存器写入数据
	TimeOut = MYI2C_TIMEOUT;
	I2C_SendData(I2Cx, Data);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_DATA;
		}
	}
	
	//结束发送
	MyI2C_ClearFlagAndStop(I2Cx);
	return MYI2C_OK;
}

//读取通信设备数据
MYI2C_STATUS MyI2C_ReadRegister(I2C_TypeDef* I2Cx, uint32_t SlaveAddress7Bit, uint32_t RegisterAddress, uint8_t* Data){
	uint32_t TimeOut = MYI2C_TIMEOUT;
	
	//检查总线是否空闲
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_BUSY;
		}
	}
	
	//start, 进入EV5
	TimeOut = MYI2C_TIMEOUT;
	I2C_GenerateSTART(I2Cx, ENABLE);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_START;
		}
	}
	
	//发送从机地址，选择通信设备, 进入EV6
	TimeOut = MYI2C_TIMEOUT;
	I2C_Send7bitAddress(I2Cx, SlaveAddress7Bit << 1, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_SLAVEADDRESS;
		}
	}
	
	//选择内部寄存器，进入EV8_2
	TimeOut = MYI2C_TIMEOUT;
	I2C_SendData(I2Cx, RegisterAddress);
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR) {  //等待数据全部移除移位寄存器，即数据真正的发出去了
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_RESADDRESS;
		}
	}
	
	//restart
	I2C_GenerateSTART(I2Cx, ENABLE);
	
	//检查是否restart成功
	TimeOut = MYI2C_TIMEOUT;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) == ERROR) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_RESTART;
		}
	}
	
	//发送读取设备地址，准备通信
	TimeOut = MYI2C_TIMEOUT;
	I2C_Send7bitAddress(I2Cx, SlaveAddress7Bit << 1, I2C_Direction_Receiver);
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == RESET) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_SEND_RECEIVE;
		}
	}
	
	
	//读取数据后ACK = 0 -> 清除ADDR -> STOP
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	//清除ADDR
	uint16_t tmp;
	tmp = I2Cx->SR1;
	tmp = I2Cx->SR2;
	(void)tmp;
	
	//STOP
	I2C_GenerateSTOP(I2Cx, ENABLE);
	
	//等待数据进入移位寄存器
	TimeOut = MYI2C_TIMEOUT;
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_RECEIVE;
		}
	}
	
	*Data = I2C_ReceiveData(I2Cx);
	
	//等待STOP成功
	TimeOut = MYI2C_TIMEOUT;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		if (TimeOut-- == 0) {
			MyI2C_ClearFlagAndStop(I2Cx);
			return MYI2C_ERROR_STOP;
		}
	}
	
	//恢复默认状态
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	return MYI2C_OK;
}
