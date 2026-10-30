#include "ServoRun.h"

ServoCalibration_t ServoPWM[ServoCount_All];  //舵机临时保存值
uint16_t PWM_NOW[ServoCount_All];  //舵机当前状态

//上电启动
void Servo_Init(void) {
	//启动读取PWM值
	uint8_t CNT = 0;
	if (ServoCalibration_Load(ServoPWM, sizeof(ServoPWM) / sizeof(ServoPWM[0] ), &CNT) == 1 ) {
		for (uint16_t i = 0; i < sizeof(ServoPWM) / sizeof(ServoPWM[0]); ++i ) {
			ServoCaliBration[i] = ServoPWM[i];	
			PWM_NOW[i] = ServoCaliBration[i].PWMOn;
		}
	}else {  //只能获取到部分校准值
		for (uint16_t i = 0; i < CNT; ++i ) {
			ServoCaliBration[i] = ServoPWM[i];	
			PWM_NOW[i] = ServoCaliBration[i].PWMOn;
		}
	}
	//else ServoCalibration_Init(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));   //默认值存在问题
	
	//转动舵机至校准值
	for (uint16_t i = 0; i < sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]) / 2; ++i ) {
		PCA9685_SETPWM(i, 0, ServoCaliBration[i].PWMOn);
		PCA9685_SETPWM(i + 14, 0, ServoCaliBration[i + 14].PWMOn);
		Delay_ms(80);
	}
	ServoColon_On();
//	ServoToggle_D(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
//	ServoToggle_D(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
//	ServoToggle_Quad(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
//	ServoToggle_Quad(PWM_NOW, sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]));
}

//关闭状态   只开启中间位 形成 "----"
void Servo_Off(void) {
	//逐个关闭
//	for (uint16_t i = 0; i < sizeof(ServoCaliBration) / sizeof(ServoCaliBration[0]); ++i ) {
//		PCA9685_SETPWM(i, 0, ServoCaliBration[i].PWMOff);
//		Delay_ms(60);
//	}
	
	//同时关闭同位
	for (int i = 0; i < 7; ++i ) {
		for (int j = 0; j < 4; ++j ) {
			uint8_t pos = i + 7 * j;
			PCA9685_SETPWM(pos, 0, ServoCaliBration[pos].PWMOff);
			PWM_NOW[pos] = ServoCaliBration[pos].PWMOff;
		}
		Delay_ms(180);
	}
	ServoColon_Off();
	
	//开启中间
	Delay_ms(200);
	for (int i = 0; i < 4; ++i ) {
		uint8_t pos = 7 + 7 * i - 1;
		PCA9685_SETPWM(pos, 0, ServoCaliBration[pos].PWMOn);
		PWM_NOW[pos] = ServoCaliBration[pos].PWMOn;
	}
}

//依次反转舵机值
void ServoToggle(uint16_t PWM_NOW[], uint16_t Count ) {
	for (uint16_t i = 0; i < Count; ++i ) {
		PWM_NOW[i] = (PWM_NOW[i] == ServoCaliBration[i].PWMOn ? ServoCaliBration[i].PWMOff : ServoCaliBration[i].PWMOn);
		PCA9685_SETPWM(i, 0, PWM_NOW[i]);
		Delay_ms(70);
	}
	ServoColon_On();
	Delay_ms(70);
	ServoColon_Off();
}

//双PCA9685依次反转舵机值
void ServoToggle_D(uint16_t PWM_NOW[], uint16_t Count ) {
	for (uint16_t i = 0; i < Count / 2; ++i ) {
		PWM_NOW[i] = (PWM_NOW[i] == ServoCaliBration[i].PWMOn ? ServoCaliBration[i].PWMOff : ServoCaliBration[i].PWMOn);
		PWM_NOW[i + 14] = (PWM_NOW[i + 14] == ServoCaliBration[i + 14].PWMOn ? ServoCaliBration[i + 14].PWMOff : ServoCaliBration[i + 14].PWMOn);
		PCA9685_SETPWM(i, 0, PWM_NOW[i]);
		PCA9685_SETPWM(i + 14, 0, PWM_NOW[i + 14]);
		Delay_ms(75);
	}
	ServoColon_On();
	Delay_ms(70);
	ServoColon_Off();
}

//四位同时反转 同段(数码管段)
void ServoToggle_Quad(uint16_t PWM_NOW[], uint16_t Count ) {
	for (uint16_t i = 0; i < ServoCount_Digit; ++i ) {
		PWM_NOW[i + 7 * 0] = (PWM_NOW[i + 7 * 0] == ServoCaliBration[i + 7 * 0].PWMOn ? ServoCaliBration[i + 7 * 0].PWMOff : ServoCaliBration[i + 7 * 0].PWMOn);
		PWM_NOW[i + 7 * 1] = (PWM_NOW[i + 7 * 1] == ServoCaliBration[i + 7 * 1].PWMOn ? ServoCaliBration[i + 7 * 1].PWMOff : ServoCaliBration[i + 7 * 1].PWMOn);
		PWM_NOW[i + 7 * 2] = (PWM_NOW[i + 7 * 2] == ServoCaliBration[i + 7 * 2].PWMOn ? ServoCaliBration[i + 7 * 2].PWMOff : ServoCaliBration[i + 7 * 2].PWMOn);
		PWM_NOW[i + 7 * 3] = (PWM_NOW[i + 7 * 3] == ServoCaliBration[i + 7 * 3].PWMOn ? ServoCaliBration[i + 7 * 3].PWMOff : ServoCaliBration[i + 7 * 3].PWMOn);
		PCA9685_SETPWM(i + 7 * 0, 0, PWM_NOW[i + 7 * 0]);
		PCA9685_SETPWM(i + 7 * 1, 0, PWM_NOW[i + 7 * 1]);
		PCA9685_SETPWM(i + 7 * 2, 0, PWM_NOW[i + 7 * 2]);
		PCA9685_SETPWM(i + 7 * 3, 0, PWM_NOW[i + 7 * 3]);
		Delay_ms(200);
	}
	ServoColon_On();
	Delay_ms(70);
	ServoColon_Off();
}

void ServoColon_On(void) {  //冒号开启
	int Change = 0;
	
	Change = PWM_NOW[28] == ServoCaliBration[28].PWMOn ? 0 : 1;
	if (Change) {
		PWM_NOW[28] = ServoCaliBration[28].PWMOn;
		PCA9685_SETPWM(28, 0, PWM_NOW[28]);
	}
	
	Change = PWM_NOW[29] == ServoCaliBration[29].PWMOn ? 0 : 1;
	if (Change) {
		PWM_NOW[29] = ServoCaliBration[29].PWMOn;
		PCA9685_SETPWM(29, 0, PWM_NOW[29]);
	}
}	

void ServoColon_Off(void) {  //冒号关闭
	int Change = 0;
	
	Change = PWM_NOW[28] == ServoCaliBration[28].PWMOff ? 0 : 1;
	if (Change) {
		PWM_NOW[28] = ServoCaliBration[28].PWMOff;
		PCA9685_SETPWM(28, 0, PWM_NOW[28]);
	}
	
	Change = PWM_NOW[29] == ServoCaliBration[29].PWMOff ? 0 : 1;
	if (Change) {
		PWM_NOW[29] = ServoCaliBration[29].PWMOff;
		PCA9685_SETPWM(29, 0, PWM_NOW[29]);
	}
}

void ServoColon_DownOn(void) {  //下冒号开启
	int Change = 0;
	
	Change = PWM_NOW[28] == ServoCaliBration[28].PWMOff ? 0 : 1;
	if (Change) {
		PWM_NOW[28] = ServoCaliBration[28].PWMOff;
		PCA9685_SETPWM(28, 0, PWM_NOW[28]);
	}
	
	Change = PWM_NOW[29] == ServoCaliBration[29].PWMOn ? 0 : 1;
	if (Change) {
		PWM_NOW[29] = ServoCaliBration[29].PWMOn;
		PCA9685_SETPWM(29, 0, PWM_NOW[29]);
	}
}


//根据断码表展示 数据     
void ServoShowNum(uint8_t Num, uint8_t Digit_POS) {
	//通道位置
	uint8_t pos = 0;
	pos += Digit_POS * 7;

	uint8_t Code;
	Code = SegmentCode[Num];	
	uint16_t PWM_Value;
	uint8_t Change = 0;
	for (int i = 0 ; i < ServoCount_Digit; ++i) {
		if (Code & (1U << i)) {  //ON
			PWM_Value = ServoCaliBration[pos].PWMOn;
			Change = !(PWM_NOW[pos] == PWM_Value);
			PWM_NOW[pos] = PWM_Value;
		}else {  //OFF
			PWM_Value = ServoCaliBration[pos].PWMOff;
			Change = !(PWM_NOW[pos] == PWM_Value);
			PWM_NOW[pos] = PWM_Value;
		}
		PCA9685_SETPWM(pos, 0, PWM_Value);
		++pos;
		if (Change) Delay_ms(50);
	}
}

//展示分 秒
void Show_MinAndSec(void) {
	ClockTime_t Time;
	if ( Get_MinAndSec(&Time)== 0) return ;
	
	//展示数据数组
	uint8_t NUM[4];
	NUM[0] = Time.Minite / 10;
	NUM[1] = Time.Minite % 10;
	NUM[2] = Time.Second / 10;
	NUM[3] = Time.Second % 10;
	
	//按位展示数据
	for (int i = 0; i < Digit; ++i ) {
		ServoShowNum(NUM[i], i);
	}
	ServoColon_On();
}

//展示时 分
void Show_HourAndMin(void) {
	ClockTime_t Time;
	if ( Get_HourAndMin(&Time)== 0) return ;
	
	//展示数据数组
	uint8_t NUM[4];
	NUM[0] = Time.Hour / 10;
	NUM[1] = Time.Hour % 10;
	NUM[2] = Time.Minite / 10;
	NUM[3] = Time.Minite % 10;
	
	//按位展示数据
	for (int i = 0; i < Digit; ++i ) {
		ServoShowNum(NUM[i], i);
	}
	ServoColon_On();
}

//展示年份
void Show_Year(void) {
	ClockTime_t Time;
	if ( Get_Year(&Time)== 0) return ;
	
	//展示数据数组
	uint8_t NUM[4];
	
	NUM[0] = Time.Year / 1000;
	Time.Year %= 1000;
	
	NUM[1] = Time.Year / 100;
	Time.Year %= 100;
	
	NUM[2] = Time.Year / 10;
	NUM[3] = Time.Year % 10;
	
	//按位展示数据
	for (int i = 0; i < Digit; ++i ) {
		ServoShowNum(NUM[i], i);
	}
	ServoColon_Off();
}

//获取月份 日期
void Show_MonAndDay(void){
	ClockTime_t Time;
	if ( Get_MonAndDay(&Time)== 0) return ;
	
	//展示数据数组
	uint8_t NUM[4];
	NUM[0] = Time.Mon / 10;
	NUM[1] = Time.Mon % 10;
	NUM[2] = Time.Day / 10;
	NUM[3] = Time.Day % 10;
	
	//按位展示数据
	for (int i = 0; i < Digit; ++i ) {
		ServoShowNum(NUM[i], i);
	}
	ServoColon_DownOn();
}  

//根据实际指令显示
void Show_Time(void) {
	if 		(DisplayMode == DISPLAY_YEAR)	  Show_Year();
	else if (DisplayMode == DISPLAY_MONTH)    Show_MonAndDay();
	else if (DisplayMode == DISPLAY_HOUR) 	  Show_HourAndMin();
	else if (DisplayMode == DISPLAY_MINUTE)   Show_MinAndSec();
}
