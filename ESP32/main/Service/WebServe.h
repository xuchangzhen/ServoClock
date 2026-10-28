#ifndef __WEBSERVE_H
#define __WEBSERVE_H

#include "CommandService.h"

#include "esp_err.h"

typedef struct {  //映射路由
    const char* URI;
    WebCommand_t Command;
} WebRoute_t;

void WebServe_Start(void);
void WebServe_Stop(void);

#endif