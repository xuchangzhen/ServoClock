#ifndef __WEBSERVE_H
#define __WEBSERVE_H

#include "esp_err.h"

typedef enum {
    WEB_COMMAND_NONE = 0,
    WEB_COMMAND_PWM_ADD_1,
    WEB_COMMAND_PWM_SUB_1,
    WEB_COMMAND_PWM_ADD_10,
    WEB_COMMAND_PWM_SUB_10,
    WEB_COMMAND_SAVE
} WebCommand_t;

void WebServe_Start(void);
void WebServe_Stop(void);

esp_err_t WebCommandQueue_Init(void);  //初始化命令队列
void WebCommand_Process(void);  //解析指令 


#endif