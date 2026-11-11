#include "PCA9685_L.h"
#include "PCA9685_R.h"
#include "MyI2C.h"
#include "Delay.h"

//内部时钟实际为28.98MHz

static MYI2C_STATUS I2C_Result;

//设置PWM频率
PCA9685_R_STATUS PCA9685_R_SetFreq50Hz(void){
	//将SLEEP置0
	uint8_t Mode1Default;
	//读取原始MODE1
	I2C_Result = MyI2C_ReadRegister(I2C1, PCA9685_R_ADDRESS, PCA9685_R_MODE1, &Mode1Default);
	if (I2C_Result != MYI2C_OK) return PCA9685_R_ERROR_READ_MODE1;
	
	//写入新的MODE1
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, PCA9685_R_MODE1, Mode1Default | 0x10);
	if (I2C_Result != MYI2C_OK) return PCA9685_R_ERROR_WRITE_MODE1;
	
	//写入新的PRESCALER
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, PCA9685_R_PRESCALER_ADDRESS, 140);
	if (I2C_Result != MYI2C_OK) return PCA9685_R_ERROR_WRITE_PRESCALER;
	
	//恢复MODE1
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, PCA9685_R_MODE1, Mode1Default & ~0x10);
	Delay_us(500);
	if (I2C_Result != MYI2C_OK) return PCA9685_R_ERROR_RECOVER_MODE1;
	
	return PCA9685_R_OK;
}

//设置脉宽
PCA9685_R_STATUS PCA9685_R_SETPWM(uint8_t Channel, uint16_t OnCount, uint16_t OffCount){
	//审查Channel和Count是否超出范围
    if (Channel > 15)
        return PCA9685_R_ERROR_SETPWM;

    if (OnCount > 4095 || OffCount > 4095)
        return PCA9685_R_ERROR_SETPWM;
	
	uint8_t On_LOW = OnCount & 0xff;
	uint8_t On_High = (OnCount >> 8) & 0x0f;
	uint8_t OffLow = OffCount & 0xff;
	uint8_t OffHigh = (OffCount >> 8) & 0x0f;
	
	uint8_t RegisterAddress = Channel * 4 + PCA9685_R_LED0;
	//写入区间值
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, RegisterAddress, On_LOW);
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, RegisterAddress + 1, On_High);
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, RegisterAddress + 2, OffLow);
	I2C_Result = MyI2C_WriteRegister(I2C1, PCA9685_R_ADDRESS, RegisterAddress + 3, OffHigh);
	
	return I2C_Result == MYI2C_OK ? PCA9685_R_OK : PCA9685_R_ERROR_SETPWM; 
}








