#include "CommandProtocol.h"

const static char* TAG = "PROT";

void CommandService_Process(void) {  //命令解析
    WebCommand_t Command;

    for (uint8_t i = 0; i < WEB_COMMAND_QUEUE_LENGTH; ++i ) {
        //队列为空立即退出
        if (!CommandService_Dequeue(&Command)) break;

        WebCommandType_t CommandType = Command.Type;

        char Frame[32];
        //解析指令
        switch (CommandType) {

            case WEB_COMMAND_PWM_START:
                UART2_SendData("#ADJUST:START@\n", strlen("#ADJUST:START@\n") - 1);
            break;

            case WEB_COMMAND_PWM_ADJUST:  //解析PWM命令
                uint8_t Length = snprintf(Frame, sizeof(Frame), "#PWM:%+d@\n", (int)Command.Value);
                if (Length > 0 && Length < sizeof(Frame)) {
                    UART2_SendData(Frame, (uint8_t)Length);
                }
                break;

            case WEB_COMMAND_SAVE:
                UART2_SendData("#SAVE@\n", strlen("#SAVE@\n"));
                break;

            case WEB_COMMAND_PWM_END:
                UART2_SendData("#ADJUST:END@\n", strlen("#ADJUST:END@\n"));
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
        
        ESP_LOGI(TAG, "Dispatched: Type=%d Value=%d",
         (int)Command.Type,
         (int)Command.Value);
    }
    // vTaskDelay(pdMS_TO_TICKS(10));
}
