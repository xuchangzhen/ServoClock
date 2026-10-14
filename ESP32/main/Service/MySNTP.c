#include "MySNTP.h"

#include <stdlib.h>
#include <time.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "CLOCK";
// extern bool SNTPStarted;
extern volatile bool WIFIConnected;

//设置时区
static void TimeZone_Init(void) {
    // 设置为中国标准时区
    setenv("TZ", "CST-8", 1);
    tzset();
}

// 初始化SNTP
void Time_Init(void) {
    //初始化时区
    TimeZone_Init();

    esp_sntp_config_t Config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org"); // 选择时间服务器

    ESP_ERROR_CHECK(esp_netif_sntp_init(&Config));
    ESP_LOGI(TAG, "SNTP_Init");

    ESP_LOGI(TAG, "Waiting for SNTP sync...");

    // 等待时间服务器同步,不成功不返回
    while (1) {
        esp_err_t Result = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000));

        if (Result == ESP_OK) {
            ESP_LOGI(TAG, "SNTP sync success");
            break;
        }

        ESP_LOGI(TAG, "SNTP sync timeout, retrying...");
    }
}

//等待wifi连接后才初始化SNTP
void Time_Get(void) {
    while (WIFIConnected == false) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }; 
    Time_Init();
}

//log输出时间信息
void Time_Print(void) {
    time_t Now;
    struct tm LocalTime;

    time(&Now);

    localtime_r(&Now, &LocalTime);

    ESP_LOGI(TAG, "Beijing Time: %02d:%02d:%02d", LocalTime.tm_hour, LocalTime.tm_min,
             LocalTime.tm_sec);
}

//将时间转化为发送至stm32格式
void TimeToSTM32(char* Data, uint8_t Len) {
    time_t Now;
    struct tm LocalTime;

    time(&Now);

    localtime_r(&Now, &LocalTime);

    snprintf(
        Data,
        Len,
        "%02d:%02d:%02d\n",
        LocalTime.tm_hour,
        LocalTime.tm_min,
        LocalTime.tm_sec
    );

    ESP_LOGI(TAG, "Send: %s", Data);
    // ESP_LOGI(TAG, "Len: %02d, TimeInit: %d", Len, (int)WIFIConnected);
}