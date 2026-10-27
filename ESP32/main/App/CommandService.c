#include "CommandService.h"
#include "MyUART.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char* TAG = "COMMAND";
static QueueSetHandle_t WebCommandQueue = NULL;

esp_err_t CommandService_Init(void) {  //初始化freertos队列
    if (WebCommandQueue != NULL) return ESP_OK;

    WebCommandQueue = xQueueCreate(WEB_COMMAND_QUEUE_LENGTH, sizeof(WebCommand_t));

    if (WebCommandQueue == NULL) {
        ESP_LOGI(TAG, "Command queue create failed");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Command queue create");

    return ESP_OK;
}

esp_err_t CommandService_Enqueue(WebCommand_t Command) {  //命令入队
    if (WebCommandQueue == NULL) return ESP_ERR_INVALID_STATE;

    if (Command <= WEB_COMMAND_NONE || Command > WEB_COMMAND_SAVE) return  ESP_ERR_INVALID_ARG;

    if ( xQueueSend(WebCommandQueue, &Command, 0) != pdPASS) {  //将指令加入队列
        ESP_LOGI(TAG, "Command queue full");
        return ESP_ERR_TIMEOUT;
    }
    ESP_LOGI(TAG, "Enqueue command: %d", (int)Command);
    return ESP_OK;
}

void CommandService_Process(void) {  //命令解析
    WebCommand_t Command;

    if (WebCommandQueue == NULL) return ;

    for (uint8_t i = 0; i < WEB_COMMAND_QUEUE_LENGTH; ++i ) {
        //队列为空立即退出
        if (xQueueReceive(WebCommandQueue, &Command, 0) != pdTRUE) break;

        //解析指令
        switch (Command) {
            
            case WEB_COMMAND_PWM_ADD_1:
                UART2_SendData("#PWM:+1@\n", 9);
                break;

            case WEB_COMMAND_PWM_SUB_1:
                UART2_SendData("#PWM:-1@\n", 9);
                break;

            case WEB_COMMAND_PWM_ADD_10:
                UART2_SendData("#PWM:+10@\n", 10);
                break;

            case WEB_COMMAND_PWM_SUB_10:
                UART2_SendData("#PWM:-10@\n", 10);
                break;

            case WEB_COMMAND_SAVE:
                UART2_SendData("#SAVE@\n", 7);
                break;

            case WEB_COMMAND_SHOW_YEAR:
                UART2_SendData("#YEAR@\n", 7);
                break;

            case WEB_COMMAND_SHOW_HOUR:
                UART2_SendData("#HOUR@\n", 7);
                break;

            case WEB_COMMAND_SHOW_MINUTE:
                UART2_SendData("#MINUTE@\n", 9);
                break;

            default:
                break;
        }
        
        ESP_LOGI(TAG, "Dispatched Command %d", (int)Command);
    }
    // vTaskDelay(pdMS_TO_TICKS(10));
}
