#include <App.h>
#include "MySNTP.h" 
#include "MyUART.h"
#include "WebServe.h"
#include "MyWIFI.h"
#include "CommandService.h"
#include "CommandProtocol.h"

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

    TickType_t LastSendTick = xTaskGetTickCount();  //获取当前tick

    while (1) {
        CommandService_Process();  //指令进程
        
        TickType_t NowTick = xTaskGetTickCount();
        if ((TickType_t)(NowTick - LastSendTick) >= pdMS_TO_TICKS(1000) && (App_Mode == APP_MODE_TIME) ) {  //1s发送一次时间
            TimeToSTM32(Data, uart_Send_Length);
            UART2_SendData(Data, strlen(Data))
            ;    
    
            LastSendTick = NowTick;
        }

        vTaskDelay(pdMS_TO_TICKS(10));

    }
}


//手动复位重启无法同步时间服务器