#include <App.h>
#include "MySNTP.h" 
#include "MyUART.h"
#include "WebServe.h"
#include "MyWIFI.h"
#include "CommandService.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

#define uart_Send_Length 32U

static const char* TAG = "APP";

void App_Init(void) {
    UART2_Init();
    ESP_ERROR_CHECK(CommandService_Init());
    WIFI_Init();
    Time_Get();

    char Data[uart_Send_Length];
    while (1) {
        CommandService_Process();
        // vTaskDelay(pdMS_TO_TICKS(50));
        TimeToSTM32(Data, uart_Send_Length);
        UART2_SendData(Data, uart_Send_Length);    

        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}


//手动复位重启无法同步时间服务器