#include "WebServe.h"
#include "CommandService.h"

#include "esp_http_server.h"
#include "esp_log.h"

#include "esp_heap_caps.h"

//引用编译时嵌入固件的网页数据
extern const char HTMLStart[] asm("_binary_index_html_start");

static const char* TAG = "WEB";
static httpd_handle_t WebServer = NULL;

// 不同 URI 对应的控制命令
static WebCommand_t PWMAdd1Command  = WEB_COMMAND_PWM_ADD_1;
static WebCommand_t PWMSub1Command  = WEB_COMMAND_PWM_SUB_1;
static WebCommand_t PWMAdd10Command = WEB_COMMAND_PWM_ADD_10;
static WebCommand_t PWMSub10Command = WEB_COMMAND_PWM_SUB_10;
static WebCommand_t PWMSaveCommand  = WEB_COMMAND_SAVE;

static WebCommand_t ShowYearCommand   = WEB_COMMAND_SHOW_YEAR;
static WebCommand_t ShowHourCommand   = WEB_COMMAND_SHOW_HOUR;
static WebCommand_t ShowMinuteCommand = WEB_COMMAND_SHOW_MINUTE; 

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
    esp_err_t Result;

    //获取uri注册传入地址
    if (Request->user_ctx == NULL) {
        return httpd_resp_send_err(
            Request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Invalid handler context"
        );
    }

    Command = *(WebCommand_t*)Request->user_ctx;
    ESP_LOGI(TAG, "Get Command: %d", (int)Command);
    
    Result = CommandService_Enqueue(Command);

    if (Result == ESP_ERR_TIMEOUT) {
        esp_err_t ResponseResult = httpd_resp_set_status(Request, "503 Service Unavailable");
        if (ResponseResult != ESP_OK) return ResponseResult;

         return httpd_resp_sendstr(Request, "Queue full");
    }

    if (Result != ESP_OK) {
        ESP_LOGI(TAG, "Command equeue failed: %s", esp_err_to_name(Result));

        return httpd_resp_send_err(
            Request,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Command service failed"
        );
    }
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

httpd_uri_t ShowYearURI = {
    .uri = "/api/Data/Year",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &ShowYearCommand
};

httpd_uri_t ShowHourURI = {
    .uri = "/api/Data/Hour",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &ShowHourCommand
};


httpd_uri_t ShowMinuteURI = {
    .uri = "/api/Data/Minute",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = &ShowMinuteCommand
};


void WebServe_Start(void) {
    httpd_config_t Config = HTTPD_DEFAULT_CONFIG();

    Config.max_uri_handlers = 16;  //添加URI注册器

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
        httpd_register_uri_handler(WebServer,&ShowYearURI);
        httpd_register_uri_handler(WebServer,&ShowHourURI);
        httpd_register_uri_handler(WebServer,&ShowMinuteURI);

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

