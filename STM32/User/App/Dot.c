#include "Dot.h"
#include "ServoCalibration.h"
#include "ServoRun.h"
#include "PCA9685_L.h"
#include "PCA9685_R.h"

void Dot_On(void) {  ////冒号开启
	PCA9685_L_SETPWM(15, 0, ServoCaliBration[28].PWMOn);
	PCA9685_R_SETPWM(15, 0, ServoCaliBration[29].PWMOn);
}

void Dot_Off(void) {  //冒号关闭
	PCA9685_L_SETPWM(15, 0, ServoCaliBration[28].PWMOff);
	PCA9685_R_SETPWM(15, 0, ServoCaliBration[29].PWMOff);
}  	

void Dot_Toggle(void) {  //冒号翻转
	PWM_NOW[28] = (PWM_NOW[28] == ServoCaliBration[28].PWMOn ? ServoCaliBration[28].PWMOff : ServoCaliBration[28].PWMOn);
	PCA9685_L_SETPWM(15, 0, PWM_NOW[28]);
	PWM_NOW[29] = (PWM_NOW[29] == ServoCaliBration[29].PWMOn ? ServoCaliBration[29].PWMOff : ServoCaliBration[29].PWMOn);
	PCA9685_L_SETPWM(15, 0, PWM_NOW[29]);
}
