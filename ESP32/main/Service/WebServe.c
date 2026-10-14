#include "WebServe.h"
#include "MyUART.h"

#include "esp_http_server.h"
#include "esp_log.h"

static const char* TAG = "WEB";
static httpd_handle_t WebServer = NULL;
static volatile WebCommand_t CurrentCommand = WEB_COMMAND_NONE;

static const char *HTML =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>Servo Calibration</title>"
    "</head>"

    "<body>"
    "<h1>Servo Calibration</h1>"

    "<button onclick=\"send('/api/pwm/sub10')\">-10</button>"
    "<button onclick=\"send('/api/pwm/sub1')\">-1</button>"
    "<button onclick=\"send('/api/pwm/add1')\">+1</button>"
    "<button onclick=\"send('/api/pwm/add10')\">+10</button>"
    "<button onclick=\"send('/api/pwm/Save')\">Save</button>"

    "<script>"
    "function send(url) {"
        "fetch(url);"
    "}"
    "</script>"

    "</body>"
    "</html>";

static esp_err_t RootHandler(httpd_req_t* Request) {
    httpd_resp_set_type (
        Request,
        "text/html"
    );

    httpd_resp_send(
        Request,
        HTML,
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

//PWM + 1
static esp_err_t PWMAdd1Handler(httpd_req_t* Request) {
    CurrentCommand = WEB_COMMAND_PWM_ADD_1;

    ESP_LOGI(TAG, "PWM + 1");

    httpd_resp_sendstr(Request, "OK");

    return ESP_OK;
}

//PWM - 1
static esp_err_t PWMSub1Handler(httpd_req_t *Request){
    CurrentCommand = WEB_COMMAND_PWM_SUB_1;

    ESP_LOGI(TAG, "PWM -1");

    httpd_resp_sendstr(Request, "OK");

    return ESP_OK;
}

//PWM + 10
static esp_err_t PWMAdd10Handler(httpd_req_t *Request){
    CurrentCommand = WEB_COMMAND_PWM_ADD_10;

    ESP_LOGI(TAG, "PWM +10");

    httpd_resp_sendstr(Request, "OK");

    return ESP_OK;
}

//PWM - 10
static esp_err_t PWMSub10Handler(httpd_req_t *Request){
    CurrentCommand = WEB_COMMAND_PWM_SUB_10;

    ESP_LOGI(TAG, "PWM -10");

    httpd_resp_sendstr(Request, "OK");

    return ESP_OK;
}

//Save
static esp_err_t PWMSaveHandler(httpd_req_t *Request){
    CurrentCommand = WEB_COMMAND_SAVE;

    ESP_LOGI(TAG, "PWM Save");

    httpd_resp_sendstr(Request, "OK");

    return ESP_OK;
}

httpd_uri_t PWMAdd1URI = {
    .uri = "/api/pwm/add1",
    .method = HTTP_GET,
    .handler = PWMAdd1Handler,
    .user_ctx = NULL
};

httpd_uri_t PWMSub1URI = {
    .uri = "/api/pwm/sub1",
    .method = HTTP_GET,
    .handler = PWMSub1Handler,
    .user_ctx = NULL
};

httpd_uri_t PWMAdd10URI = {
    .uri = "/api/pwm/add10",
    .method = HTTP_GET,
    .handler = PWMAdd10Handler,
    .user_ctx = NULL
};

httpd_uri_t PWMSub10URI = {
    .uri = "/api/pwm/sub10",
    .method = HTTP_GET,
    .handler = PWMSub10Handler,
    .user_ctx = NULL
};

httpd_uri_t PWMSaveURI = {
    .uri = "/api/pwm/Save",
    .method = HTTP_GET,
    .handler = PWMSaveHandler,
    .user_ctx = NULL
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