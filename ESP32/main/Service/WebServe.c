#include "WebServe.h"

#include "esp_http_server.h"
#include "esp_log.h"

#include "esp_heap_caps.h"

static const WebRoute_t CommandRoutes[] = {
    {"/api/pwm/start", {WEB_COMMAND_PWM_START, 0}},
    {"/api/pwm/add1", {WEB_COMMAND_PWM_ADJUST, +1}},
    {"/api/pwm/sub1", {WEB_COMMAND_PWM_ADJUST, -1}},
    {"/api/pwm/add10", {WEB_COMMAND_PWM_ADJUST, +10}},
    {"/api/pwm/sub10", {WEB_COMMAND_PWM_ADJUST, -10}},
    {"/api/pwm/Save", {WEB_COMMAND_SAVE, 0}},
    {"/api/pwm/end", {WEB_COMMAND_PWM_END, 0}},
    {"/api/Data/Year", {WEB_COMMAND_SHOW_YEAR, 0}},
    {"/api/Data/Hour", {WEB_COMMAND_SHOW_HOUR, 0}},
    {"/api/Data/Minute", {WEB_COMMAND_SHOW_MINUTE, 0}}
};

//引用编译时嵌入固件的网页数据
extern const char HTMLStart[] asm("_binary_index_html_start");

static const char* TAG = "WEB";
static httpd_handle_t WebServer = NULL;


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
    WebCommand_t Command = {WEB_COMMAND_NONE, 0};
    esp_err_t Result;

    for (int i = 0; i < sizeof(CommandRoutes) / sizeof(WebRoute_t); ++i) {
        if (strcmp(Request->uri, CommandRoutes[i].URI) == 0) {
            Command = CommandRoutes[i].Command;
            break;
        }
    }

    ESP_LOGI(TAG, "Get Command: %d, Value: %+d", (int)Command.Type, (int)Command.Value);
    
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
httpd_uri_t CommandURI = {  //同一请求入口
    .uri = "/api/*",
    .method = HTTP_POST,
    .handler = PWMCommandHandler,
    .user_ctx = NULL
};


void WebServe_Start(void) {  //启动WebServe
    httpd_config_t Config = HTTPD_DEFAULT_CONFIG();

    Config.max_uri_handlers = 16;  //添加URI注册器
    Config.uri_match_fn = httpd_uri_match_wildcard;  //通配符处理请求

    httpd_uri_t RootURI = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = RootHandler,
        .user_ctx = NULL
    };

    if (WebServer != NULL) return ;

    if (httpd_start(&WebServer, &Config) == ESP_OK) {
        httpd_register_uri_handler(WebServer, &RootURI);
        esp_err_t Result = httpd_register_uri_handler(WebServer,&CommandURI);

        if (Result != ESP_OK) {
            ESP_LOGI(TAG, "%s", esp_err_to_name(Result));
            return ;
        }   

        ESP_LOGI(TAG, "Web Serve Started");

    }else {
        ESP_LOGI(TAG, "Web Serve Start Failed");
    }
}

void WebServe_Stop(void) {  //停止WebServe
    if (WebServer == NULL) return;
    
    httpd_stop(WebServer);
    WebServer = NULL;

    ESP_LOGI(TAG, "Stopped");
}

