#ifndef __WEBSERVE_H
#define __WEBSERVE_H

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

WebCommand_t WebServer_GetCommand(void);  //查看网页command类型
void WebCommand_Process(void);             


#endif