#include "MyUART.h"

#include <string.h>

#include "driver/uart.h"
#include "nvs_flash.h"

void UART2_Init(void) {
    // 配置引脚参数
    uart_config_t Config = {.baud_rate = 9600,
                            .data_bits = UART_DATA_8_BITS,
                            .parity = UART_PARITY_DISABLE,
                            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                            .stop_bits = UART_STOP_BITS_1,
                            .source_clk = UART_SCLK_DEFAULT};
    // 配置引脚
    ESP_ERROR_CHECK(uart_param_config(STM32_UART, &Config));

    ESP_ERROR_CHECK(uart_set_pin(STM32_UART, STM32_UART_TX_PIN, STM32_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(STM32_UART, 256, 0, 0, NULL, 0));
}

void UART2_SendData(char* Data, uint8_t Len) {
    //检查tx是否空闲


    uart_write_bytes(STM32_UART, Data, Len);
}

