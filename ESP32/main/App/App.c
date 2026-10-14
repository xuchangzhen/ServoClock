#include <App.h>
#include "MyWIFI.h"
#include "MySNTP.h" 
#include "MyUART.h"
#include "WebServe.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "APP";

void App_Init(void) {
    UART2_Init();
    WIFI_Init();
    Time_Get();
                
    char Data[16];
    while (1) {
        // WebCommand_Process();
        // vTaskDelay(pdMS_TO_TICKS(50));
        TimeToSTM32(Data, 10);
        UART2_SendData(Data, 9);    

        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}


//手动复位重启无法同步时间服务器