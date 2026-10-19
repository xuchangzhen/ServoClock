#include "WebServe.h"
#include "MyUART.h"

#include "esp_http_server.h"
#include "esp_log.h"

//引用编译时嵌入固件的网页数据
extern const char HTMLStart[] asm("_binary_index_html_start");

static const char* TAG = "WEB";
static httpd_handle_t WebServer = NULL;
static volatile WebCommand_t CurrentCommand = WEB_COMMAND_NONE;

// 不同 URI 对应的控制命令
static WebCommand_t PWMAdd1Command = WEB_COMMAND_PWM_ADD_1;
static WebCommand_t PWMSub1Command = WEB_COMMAND_PWM_SUB_1;
static WebCommand_t PWMAdd10Command = WEB_COMMAND_PWM_ADD_10;
static WebCommand_t PWMSub10Command = WEB_COMMAND_PWM_SUB_10;
static WebCommand_t PWMSaveCommand = WEB_COMMAND_SAVE;


static esp_err_t RootHandler(httpd_req_t* Request) {
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
    if (Request->user_ctx == NULL) {
        return httpd_resp_send_err(
            Request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Invalid command"
        );
    }

    Command = *(WebCommand_t*)Request->user_ctx;
    CurrentCommand = Command;
    
    ESP_LOGI(TAG, "Receive command: %d", (int)Command);

    httpd_resp_sendstr(Request, "OK");

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

WebCommand_t WebServer_GetCommand(void) {
    WebCommand_t Command;

    Command = CurrentCommand;

    CurrentCommand = WEB_COMMAND_NONE;

    return Command;
}

//发送校准数据
void WebCommand_Process(void) {
    WebCommand_t Command;

    Command = WebServer_GetCommand();

    switch (Command) {

    case WEB_COMMAND_PWM_ADD_1:
        UART2_SendData("PWM:+1\n", 7);
        ESP_LOGI(TAG, "APP received: PWM +1");
        break;

    case WEB_COMMAND_PWM_SUB_1:
        UART2_SendData("PWM:-1\n", 7);
        ESP_LOGI(TAG, "APP received: PWM -1");
        break;

    case WEB_COMMAND_PWM_ADD_10:
        UART2_SendData("PWM:+10\n", 8);
        ESP_LOGI(TAG, "APP received: PWM +10");
        break;

    case WEB_COMMAND_PWM_SUB_10:
        UART2_SendData("PWM:-10\n", 8);
        ESP_LOGI(TAG, "APP received: PWM -10");
        break;

    case WEB_COMMAND_SAVE:
        UART2_SendData("Save\n", 5);
        ESP_LOGI(TAG, "APP received: Save");
    default:
        break;
    }

    // vTaskDelay(pdMS_TO_TICKS(10));
}