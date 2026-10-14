#ifndef __MYSNTP_H
#define __MYSNTP_H

#include <stdint.h>

void Time_Get(void);
void Time_Print(void); 
void TimeToSTM32(char* Data, uint8_t Len);

#endif
