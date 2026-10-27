#ifndef __WEBSERVE_H
#define __WEBSERVE_H

#include "CommandService.h"

#include "esp_err.h"

typedef struct {  //映射路由
    const char* URI;
    WebCommand_t Command;
} WebRoute_t;

static const WebRoute_t CommandRoutes[] = {
    {"/api/pwm/add1", WEB_COMMAND_PWM_ADD_1},
    {"/api/pwm/sub1", WEB_COMMAND_PWM_SUB_1},
    {"/api/pwm/add10", WEB_COMMAND_PWM_ADD_10},
    {"/api/pwm/sub10", WEB_COMMAND_PWM_SUB_10},
    {"/api/pwm/Save", WEB_COMMAND_SAVE},
    {"/api/Data/Year", WEB_COMMAND_SHOW_YEAR},
    {"/api/Data/Hour", WEB_COMMAND_SHOW_HOUR},
    {"/api/Data/Minute", WEB_COMMAND_SHOW_MINUTE}
};

void WebServe_Start(void);
void WebServe_Stop(void);

#endif