#include <stdbool.h>

#include "MyWIFI.h"
#include "WebServe.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "nvs_flash.h"


static const char *TAG = "CLOCK";
volatile bool WIFIConnected = false;
// bool SNTPStarted = false;

static void WIFI_EventHandler(void *Arg, esp_event_base_t EventBase, int32_t EventID, void *EventData){
    // wifi连接事件
    if (EventBase == WIFI_EVENT && EventID == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    // wifi断连恢复事件
    if (EventBase == WIFI_EVENT && EventID == WIFI_EVENT_STA_DISCONNECTED){
        wifi_event_sta_disconnected_t *Event;

        Event = (wifi_event_sta_disconnected_t *)EventData;

        ESP_LOGW(TAG, "WIFI disconnected, reason = %d", Event->reason);
        esp_wifi_connect();
    }

    // 获取ip地址
    if (EventBase == IP_EVENT && EventID == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t *Event;

        Event = (ip_event_got_ip_t *)EventData;

        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&Event->ip_info.ip));
            
        //只初始化一次SNTP
        // if (SNTPStarted == false){
        //     SNTPStarted = true;
        // }
        WIFIConnected = true;
        WebServe_Start();
    }
}

void WIFI_Init(void){
    esp_err_t Result;

    // 初始化nvs
    Result = nvs_flash_init();

    if (Result == ESP_ERR_NVS_NO_FREE_PAGES || Result == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // 初始化网络接口和系统事件
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 创建STA网络接口
    esp_netif_create_default_wifi_sta();

    // 初始化wifi驱动
    wifi_init_config_t Config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&Config));
    
    ESP_LOGI(TAG, "WIFI_Init");

    // 注册wifi和ip事件
    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            WIFI_EventHandler,
            NULL));

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            ESP_EVENT_ANY_ID,
            WIFI_EventHandler,
            NULL));

    
    // 配置连接wifi
    wifi_config_t WIFIConfig = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD}};

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &WIFIConfig));

    // 启动wifi，后续出现问题由事件函数触发
    ESP_ERROR_CHECK(esp_wifi_start());
}