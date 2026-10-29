#ifndef __COMMANDSERVICE_H
#define __COMMANDSERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#define WEB_COMMAND_QUEUE_LENGTH 8U

typedef enum{  //命令类型
    WEB_COMMAND_NONE = 0,
    WEB_COMMAND_APP_ON,
    WEB_COMMAND_APP_OFF,
    WEB_COMMAND_PWM_CAL,
    WEB_COMMAND_PWM_ADJUST,
    WEB_COMMAND_SHOW_YEAR,
    WEB_COMMAND_SHOW_MONTH,
    WEB_COMMAND_SHOW_HOUR,
    WEB_COMMAND_SHOW_MINUTE,
    WEB_COMMAND_PWM_END,
    WEB_COMMAND_SAVE,

    WEB_COMMAND_COUTN
} WebCommandType_t;


typedef struct {  //带参数命令结构体
    WebCommandType_t  Type;
    int16_t           Value;
} WebCommand_t;

typedef enum{
    APP_MODE_TIME,
    APP_MODE_CAL
} AppMode_t;

extern AppMode_t App_Mode;

esp_err_t CommandService_Init(void);                          //初始化任务队列
esp_err_t CommandService_Enqueue(WebCommand_t Command);       //命令入队
bool CommandService_Dequeue(WebCommand_t* Command);            //命令出队

#endif
