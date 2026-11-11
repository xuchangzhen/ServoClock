#include "W25Q64.h"
#include "MYSPI.h"

#define W25Q64_CMD_WRITE_ENABLE    		0x06
#define W25Q64_CMD_WRITE_DISANABLE    	0x04
#define W25Q64_CMD_SECTOR_ERASE    		0x20
#define W25Q64_CMD_PAGE_PROGRAM			0x02
#define W25Q64_CMD_READ_DATA			0x03
#define W25Q64_CMD_READ_STATUS1 		0x05

#define W25Q64_TIMEOUT					100000UL
#define W25Q64_PAGE_SIZE				256U
#define W25Q64_SECTOR_SIZE				4096UL
#define W25Q64_CAPASITY					8000000UL

static uint8_t W25Q64_SectorBuffer[W25Q64_SECTOR_SIZE];

//写使能
W25Q64_STATUS W25Q64_WriteEnable(void) {
	MySPI_Start();
	//W25Q64_STATUS Result;
	
	if (MySPI_SendByte(W25Q64_CMD_WRITE_ENABLE) != MYSPI_OK) {
		MySPI_Stop();
		return W25Q64_ERROR_WRITE_ENABLE;
	}
	
	MySPI_Stop();
	return W25Q64_OK;
}

//写失能
W25Q64_STATUS W25Q64_WriteDisable(void) {
	MySPI_Start();
	//W25Q64_STATUS Result;
	
	if (MySPI_SendByte(W25Q64_CMD_WRITE_DISANABLE) != MYSPI_OK) {
		MySPI_Stop();
		return W25Q64_ERROR_WRITE_DISABLE;
	}
	
	MySPI_Stop();
	return W25Q64_OK;
}

//读取状态寄存器1
W25Q64_STATUS W25Q64_ReadStatus1(uint8_t* Status) {
	MySPI_Start();
	
	if (MySPI_SendByte(W25Q64_CMD_READ_STATUS1) != MYSPI_OK) {
		MySPI_Stop();
		return W25Q64_ERROR_READ_STATUS;
	}
	
	if (MySPI_ReceiceByte(Status) != MYSPI_OK) {
		MySPI_Stop();
		return W25Q64_ERROR_READ_STATUS;
	}
	
	MySPI_Stop();
	return W25Q64_OK;
}

//等待BUSY
W25Q64_STATUS W25Q64_WaitBusy(void) {
	uint8_t Status;
	uint32_t Timeout = W25Q64_TIMEOUT;
	W25Q64_STATUS Result;
	
	do{
		Result = W25Q64_ReadStatus1(&Status);
		if (Result != W25Q64_OK) return Result;
		
		if ((Status & 0x01) == 0) return W25Q64_OK;
	}while (Timeout--);
	
	return W25Q64_ERROR_TIMEOUT;
}

void W25Q64_Stop(void) {
	W25Q64_WriteDisable();
	MySPI_Stop();
}

//W25Q64_STATUS W25Q64_BackResult(W25Q64_STATUS Result) {
//	W25Q64_Stop();
//	return Result;
//}

//查看地址是否合法
bool W25Q64_CheckEraseAddress(uint32_t Address) {
	uint32_t Check = Address & 0xfff;
	if (Check != 0) return false;
	return true;
}

bool W25Q64_AllFF(void);

//发送数据
W25Q64_STATUS W25Q64_SendData(uint8_t Data) {
	//W25Q64_STATUS Result;
	
	//uint16_t Temp;
	if (MySPI_SendByte(Data) != MYSPI_OK) {
		MySPI_Stop();
		return W25Q64_ERROR_SendData;
	};
	
	return W25Q64_OK;
}

//扇区清除
W25Q64_STATUS W25Q64_SectorErase(uint32_t Address) {
	W25Q64_STATUS Result;
	//检查地址是否为扇区的开头
	//Result = W25Q64_CheckEraseAddress(Address) ? W25Q64_OK : W25Q64_ERROR_ILLEGEL_SECTOR_ADDRESS;
//	if (Result != W25Q64_OK) {
//		MySPI_Stop();
//		return Result;
//	}
	
	Result = W25Q64_WriteEnable();
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	MySPI_Start();
	
	Result = W25Q64_SendData(W25Q64_CMD_SECTOR_ERASE);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	Result = W25Q64_SendData(Address >> 16);
	Result = W25Q64_SendData(Address >> 8);
	Result = W25Q64_SendData(Address);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	MySPI_Stop();
	
	return W25Q64_WaitBusy();
}

//页编程
W25Q64_STATUS W25Q64_PageProgram(uint32_t Address, uint8_t* Data, uint32_t Length) {
	W25Q64_STATUS Result;
	
//	//扇区清除
//	Result = W25Q64_SectorErase(Address);
//	if (Result != W25Q64_OK) {
//		MySPI_Stop();
//		return Result;
//	}
//	
	//写使能
	Result = W25Q64_WriteEnable();
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	MySPI_Start();
	
	//发送写指令
	Result = W25Q64_SendData(W25Q64_CMD_PAGE_PROGRAM);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	//发送地址
	Result = W25Q64_SendData(Address >> 16);
	Result = W25Q64_SendData(Address >> 8);
	Result = W25Q64_SendData(Address);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	while (Length) {
		W25Q64_SendData(*Data);
		++Data;
		--Length;
	}
	
	MySPI_Stop();
	
	return W25Q64_WaitBusy();
}

//读取Page信息
W25Q64_STATUS W25Q64_ReadData(uint32_t Address, uint8_t* Data, uint32_t Length){
	MySPI_Start();
	W25Q64_STATUS Result;
	
	//发送读指令
	Result = W25Q64_SendData(W25Q64_CMD_READ_DATA);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	Result = W25Q64_SendData(Address >> 16);
	Result = W25Q64_SendData(Address >> 8);
	Result = W25Q64_SendData(Address);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	while (Length--) {
		if (MySPI_ReceiceByte(Data++) != MYSPI_OK) {
			MySPI_Stop();
			return Result;
		}
	}
	
	MySPI_Stop();
	
	return W25Q64_OK;
}

//跨页编程(不保留旧数据)
W25Q64_STATUS W25Q64_AcrossPageProgram(uint32_t Address, uint8_t* Data, uint32_t Length) {
	uint16_t WriteLength;
	W25Q64_STATUS Result;
	
	while (Length > 0) {
		uint16_t PageOffset = Address & 0xff;
		uint32_t Remainting = W25Q64_PAGE_SIZE - PageOffset;
		if (Remainting > Length) {
			WriteLength = Length;
		}else {
			WriteLength = Remainting;
		}
		Result = W25Q64_PageProgram(Address, Data, WriteLength);
		if (Result != W25Q64_OK) return Result;
		
		Length -= WriteLength;
		Data += WriteLength;
		Address += WriteLength;
	}
	return W25Q64_OK;
}

bool W25Q64_CheckAllFF(uint8_t* Data, uint16_t Length) {
	for (uint16_t i = 0; i < Length; ++i ) {
		if (Data[i] != 0xff) return 0;
	}
	return 1;
};

//跨页智能写入(保留旧数据)
W25Q64_STATUS W25Q64_WriteDataInSector(uint32_t Address, uint8_t* Data, uint16_t Length) {
	//MySPI_Start();
	W25Q64_STATUS Result;
	
	if (Length == 0) return W25Q64_ERROR_WRITEDATA_INSECTOR_LENGTH;
	//if (W25Q64_CheckEraseAddress(Address) != 1) return W25Q64_ERROR_ADDRESS;
	
	uint32_t Sector_Address = Address & 0xfffff000; 
	uint32_t Sector_Offset = Address - Sector_Address;
	
	if (Length > W25Q64_SECTOR_SIZE - Sector_Offset) return W25Q64_ERROR_SECTOR_BOUNDARY;
	
	//读取原扇区数据
	Result = W25Q64_ReadData(Address, W25Q64_SectorBuffer, W25Q64_SECTOR_SIZE);
	if (Result != W25Q64_OK) {
		MySPI_Stop();
		return Result;
	}
	
	bool NeedErase = 0;
	for (uint16_t i = 0; i < Length; ++i ) {
		if ((Data[i] & W25Q64_SectorBuffer[i]) != Data[i]) {
			NeedErase = 1;
			break;
		}
	}
	if (NeedErase == 0) {
		W25Q64_AcrossPageProgram(Address, Data, Length);
	}else {  //不需要擦除扇区
		
		//将数据写入缓冲区
		for (uint16_t i = 0; i < Length; ++i) W25Q64_SectorBuffer[Sector_Offset + i] = Data[i];  //将数据写入实际扇区位置
		
		//扇区清除
		Result = W25Q64_SectorErase(Address);
		if (Result != W25Q64_OK) {
			MySPI_Stop();
			return Result;
		}
		for (uint32_t PageOffset = 0; PageOffset < W25Q64_SECTOR_SIZE; PageOffset += W25Q64_PAGE_SIZE) {
			if (W25Q64_CheckAllFF(&W25Q64_SectorBuffer[PageOffset], W25Q64_PAGE_SIZE) == 1) continue;
			
			Result = W25Q64_PageProgram(Sector_Address + PageOffset, &W25Q64_SectorBuffer[PageOffset], W25Q64_PAGE_SIZE);
			if (Result != W25Q64_OK) return Result;
		}
	}
	return W25Q64_OK;
}

//跨扇区写入
W25Q64_STATUS W25Q64_WriteData(uint32_t Address, uint8_t* Data, uint16_t Length){
	if (Length == 0) return W25Q64_ERROR_WRITEDATA_LENGTH;
	//if (W25Q64_CheckEraseAddress(Address) != 1) return W25Q64_ERROR_ADDRESS;
	
	uint32_t AddressOffset = Address & 0xfffUL;
	uint32_t SectorRemaining = W25Q64_SECTOR_SIZE - AddressOffset;
	uint16_t WriteLength;
	W25Q64_STATUS Result;
	while (Length > 0) {
		if (Length > SectorRemaining) WriteLength = (uint16_t)SectorRemaining;
		else 						  WriteLength = Length;
		
		Result = W25Q64_WriteDataInSector(Address, Data, WriteLength);
		
		if (Result != W25Q64_OK) return Result;
		
		Address += WriteLength;
		Data += WriteLength;
		Length -= WriteLength;
	}
	return W25Q64_OK;
}




