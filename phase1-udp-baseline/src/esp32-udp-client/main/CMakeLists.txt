if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.private/wifi_credentials.h")
    message(FATAL_ERROR "Missing local .private/wifi_credentials.h; provide TRIAL_WIFI_SSID and TRIAL_WIFI_PASSWORD string literals. Do not commit credentials.")
endif()

idf_component_register(SRCS "main.c"
    INCLUDE_DIRS "." PRIV_INCLUDE_DIRS "../.private"
    PRIV_REQUIRES esp_wifi esp_event esp_netif nvs_flash lwip freertos log esp_common heap)
