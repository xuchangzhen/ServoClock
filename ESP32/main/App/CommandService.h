#ifndef __COMMANDSERVICE_H
#define __COMMANDSERVICE_H

#include "esp_err.h"

#define WEB_COMMAND_QUEUE_LENGTH 8U

typedef enum {  //命令类型
    WEB_COMMAND_NONE = 0,
    WEB_COMMAND_PWM_ADD_1,
    WEB_COMMAND_PWM_SUB_1,
    WEB_COMMAND_PWM_ADD_10,
    WEB_COMMAND_PWM_SUB_10,
    WEB_COMMAND_SHOW_YEAR,
    WEB_COMMAND_SHOW_HOUR,
    WEB_COMMAND_SHOW_MINUTE,
    WEB_COMMAND_SAVE
} WebCommand_t;

esp_err_t CommandService_Init(void);                          //初始化任务队列
esp_err_t CommandService_Enqueue(WebCommand_t Command);       //命令入队
void CommandService_Process(void);                            //命令解析

#endif
