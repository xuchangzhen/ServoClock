#ifndef __MYWIFI_H
#define __MYWIFI_H

#define WIFI_SSID "esp_for"
#define WIFI_PASSWORD "xcz88888888"

void WIFI_Init(void);  //初始化wifi连接
bool DNS_Change(void);  //修改dns服务器

#endif 
