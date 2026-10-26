idf_component_register(
    SRC_DIRS "." "Service" "Driver" "App" "BSP"
    INCLUDE_DIRS "." "Service" "Driver" "App" "BSP"
    REQUIRES esp_driver_uart esp_wifi esp_event esp_netif nvs_flash log PRIV_REQUIRES esp_http_server

    EMBED_TXTFILES
        "Web/index.html"
    
)
