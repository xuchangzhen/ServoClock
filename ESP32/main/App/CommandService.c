#include "CommandService.h"
#include "MyUART.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char* TAG = "COMMAND";
static QueueHandle_t WebCommandQueue = NULL;

AppMode_t App_Mode = APP_MODE_TIME;

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

    if (Command.Type <= WEB_COMMAND_NONE || Command.Type >= WEB_COMMAND_COUTN) {
        ESP_LOGE(TAG, "Invalid CommandType: %d, Max: %d",
            (int)Command.Type, (int)WEB_COMMAND_COUTN);
            return  ESP_ERR_INVALID_ARG;
    }

    if ( xQueueSend(WebCommandQueue, &Command, 0) != pdPASS) {  //将指令加入队列
        ESP_LOGI(TAG, "Command queue full");
        return ESP_ERR_TIMEOUT;
    }
    ESP_LOGI(TAG, "Enqueue Command: %d, Value: %+d", (int)Command.Type, (int)Command.Value);
    return ESP_OK;
}

bool CommandService_Dequeue(WebCommand_t* Command) {  //命令出队
    if (WebCommandQueue == NULL || Command == NULL) return false;

    return xQueueReceive(WebCommandQueue, Command, 0) == pdTRUE;
}


