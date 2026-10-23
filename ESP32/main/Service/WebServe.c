#include "WebServe.h"
#include "MyUART.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/queue.h"

#include "esp_heap_caps.h"

#define WEB_COMMAND_QUEUE_LENGTH 8U

//引用编译时嵌入固件的网页数据
extern const char HTMLStart[] asm("_binary_index_html_start");

static const char* TAG = "WEB";
static httpd_handle_t WebServer = NULL;
static QueueSetHandle_t WebCommandQueue = NULL;

// 不同 URI 对应的控制命令
static WebCommand_t PWMAdd1Command = WEB_COMMAND_PWM_ADD_1;
static WebCommand_t PWMSub1Command = WEB_COMMAND_PWM_SUB_1;
static WebCommand_t PWMAdd10Command = WEB_COMMAND_PWM_ADD_10;
static WebCommand_t PWMSub10Command = WEB_COMMAND_PWM_SUB_10;
static WebCommand_t PWMSaveCommand = WEB_COMMAND_SAVE;

esp_err_t WebCommandQueue_Init(void) {  //初始化freertos队列
    if (WebCommandQueue != NULL) return ESP_OK;

    WebCommandQueue = xQueueCreate(WEB_COMMAND_QUEUE_LENGTH, sizeof(WebCommand_t));

    if (WebCommandQueue == NULL) {
        ESP_LOGI(TAG, "Command queue create failed");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Command queue create");

    return ESP_OK;
}



static esp_err_t RootHandler(httpd_req_t* Request) {  //网页根目录请求
    esp_err_t Result;
    
    //设置http响应类型
    Result = httpd_resp_set_type (
        Request,
        "text/html"
    );

    if (Result != ESP_OK) return Result;

    return httpd_resp_sendstr(Request, HTMLStart);
}

//http请求处理
static esp_err_t PWMCommandHandler(httpd_req_t* Request) {
    WebCommand_t Command;

    //获取uri注册传入地址
    if (Request->user_ctx == NULL || WebCommandQueue == NULL) {
        return httpd_resp_send_err(
            Request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Command Serve unavailable"
        );
    }

    Command = *(WebCommand_t*)Request->user_ctx;
    if ( xQueueSend(WebCommandQueue, &Command, 0) != pdPASS) {  //将指令加入队列
        ESP_LOGI(TAG, "Command queue full");

        esp_err_t Result = httpd_resp_set_status(Request, "503 Service Unavailable");

        if (Result != ESP_OK) return Result;

        return httpd_resp_sendstr(Request, "Queue full");
    }  
    
    ESP_LOGI(TAG, "Queue command: %d", (int)Command);

    httpd_resp_sendstr(Request, "QUEUE");

    return ESP_OK;
}


//注册http接口
httpd_uri_t PWMAdd1URI = {
    .uri = "/api/pwm/add1",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &PWMAdd1Command
};

httpd_uri_t PWMSub1URI = {
    .uri = "/api/pwm/sub1",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &PWMSub1Command
};

httpd_uri_t PWMAdd10URI = {
    .uri = "/api/pwm/add10",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &PWMAdd10Command
};

httpd_uri_t PWMSub10URI = {
    .uri = "/api/pwm/sub10",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &PWMSub10Command
};

httpd_uri_t PWMSaveURI = {
    .uri = "/api/pwm/Save",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &PWMSaveCommand
};


void WebServe_Start(void) {
    httpd_config_t Config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t RootURI = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = RootHandler,
        .user_ctx = NULL
    };

    if (WebServer != NULL) return ;

    if (httpd_start(&WebServer, &Config) == ESP_OK) {
        httpd_register_uri_handler(WebServer, &RootURI);
        httpd_register_uri_handler(WebServer,&PWMAdd1URI);
        httpd_register_uri_handler(WebServer,&PWMSub1URI);
        httpd_register_uri_handler(WebServer,&PWMAdd10URI);
        httpd_register_uri_handler(WebServer,&PWMSub10URI);
        httpd_register_uri_handler(WebServer,&PWMSaveURI);

        ESP_LOGI(TAG, "Web Serve Started");

    }else {
        ESP_LOGI(TAG, "Web Serve Start Failed");
    }
}

void WebServe_Stop(void) {
    if (WebServer == NULL) return;
    
    httpd_stop(WebServer);
    WebServer = NULL;

    ESP_LOGI(TAG, "Stopped");
}

//发送校准数据
void WebCommand_Process(void) {
    WebCommand_t Command;

    if (WebCommandQueue == NULL) return ;

    for (uint8_t i = 0; i < WEB_COMMAND_QUEUE_LENGTH; ++i ) {
        //队列为空立即退出
        if (xQueueReceive(WebCommandQueue, &Command, 0) != pdTRUE) break;

        //解析指令
        switch (Command) {
            
            case WEB_COMMAND_PWM_ADD_1:
                UART2_SendData("PWM:+1\n", 7);
                break;

            case WEB_COMMAND_PWM_SUB_1:
                UART2_SendData("PWM:-1\n", 7);
                break;

            case WEB_COMMAND_PWM_ADD_10:
                UART2_SendData("PWM:+10\n", 8);
                break;

            case WEB_COMMAND_PWM_SUB_10:
                UART2_SendData("PWM:-10\n", 8);
                break;

            case WEB_COMMAND_SAVE:
                UART2_SendData("Save\n", 5);
                break;
            default:
                break;
        }
        ESP_LOGI(TAG, "Dispatched Command %d", (int)Command);
    }
    // vTaskDelay(pdMS_TO_TICKS(10));
}