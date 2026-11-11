#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "lwip/inet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "wifi_credentials.h"

static const char TAG[] = "udp_trial";
static const char SERVER_IPV4[] = "192.168.0.170";
static const uint16_t SERVER_PORT = 56830;
static const uint8_t PING_PAYLOAD[4] = {'P', 'I', 'N', 'G'};
static const uint8_t PONG_PAYLOAD[4] = {'P', 'O', 'N', 'G'};
static const unsigned int MAX_EXCHANGE_ATTEMPTS = 20;
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 30000;
static const long RECEIVE_TIMEOUT_SECONDS = 2;
static const uint32_t EXCHANGE_INTERVAL_MS = 1000;
static const EventBits_t WIFI_READY_BIT = BIT0;
static const EventBits_t WIFI_FAILED_BIT = BIT1;
static const EventBits_t WIFI_STARTED_BIT = BIT2;
static EventGroupHandle_t wifi_events = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    (void)arg;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        xEventGroupSetBits(wifi_events, WIFI_STARTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        const wifi_event_sta_disconnected_t *event = event_data;
        ESP_LOGE(TAG, "Wi-Fi disconnected: reason=%u", (unsigned int)event->reason);
        xEventGroupClearBits(wifi_events, WIFI_READY_BIT);
        xEventGroupSetBits(wifi_events, WIFI_FAILED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if ((xEventGroupGetBits(wifi_events) & WIFI_FAILED_BIT) == 0) {
            const ip_event_got_ip_t *event = event_data;
            ESP_LOGI(TAG, "IPv4 acquired: " IPSTR, IP2STR(&event->ip_info.ip));
            xEventGroupSetBits(wifi_events, WIFI_READY_BIT);
        }
    }
}

void app_main(void)
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifi_config = {0};
    esp_netif_t *station_netif = NULL;
    esp_event_handler_instance_t wifi_handler_instance = NULL;
    esp_event_handler_instance_t ip_handler_instance = NULL;
    bool nvs_initialized = false;
    bool event_loop_created = false;
    bool wifi_initialized = false;
    bool wifi_started = false;
    bool event_group_delete_safe = true;
    int socket_fd = -1;
    struct sockaddr_in server_address = {0};
    struct sockaddr_in local_address = {0};
    socklen_t local_address_length = sizeof(local_address);
    struct timeval receive_timeout = {.tv_sec = RECEIVE_TIMEOUT_SECONDS, .tv_usec = 0};
    uint8_t receive_buffer[64];
    unsigned int attempted_exchanges = 0;
    unsigned int successful_exchanges = 0;
    EventBits_t wifi_bits;
    TickType_t wifi_wait_started_at;
    const TickType_t wifi_wait_budget = pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS);
    TickType_t wifi_wait_remaining;
    esp_err_t result;

    ESP_LOGI(TAG, "ESP32-S3 UDP trial: peer=%s:%u maximum_attempts=%u",
             SERVER_IPV4, (unsigned int)SERVER_PORT, MAX_EXCHANGE_ATTEMPTS);
    ESP_LOGI(TAG, "Internal heap before Wi-Fi: free=%u bytes",
             (unsigned int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

    result = nvs_flash_init();
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init: %s (no automatic erase)", esp_err_to_name(result));
        goto cleanup;
    }
    nvs_initialized = true;
    result = esp_netif_init();
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_netif_init: %s", esp_err_to_name(result));
        goto cleanup;
    }
    result = esp_event_loop_create_default();
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_loop_create_default: %s", esp_err_to_name(result));
        goto cleanup;
    }
    event_loop_created = true;
    wifi_events = xEventGroupCreate();
    if (wifi_events == NULL) {
        ESP_LOGE(TAG, "Event-group allocation failed");
        goto cleanup;
    }
    station_netif = esp_netif_create_default_wifi_sta();
    if (station_netif == NULL) {
        ESP_LOGE(TAG, "Station-interface creation failed");
        goto cleanup;
    }
    result = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                wifi_event_handler, NULL, &wifi_handler_instance);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Register Wi-Fi handler: %s", esp_err_to_name(result));
        goto cleanup;
    }
    result = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                wifi_event_handler, NULL, &ip_handler_instance);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Register IP handler: %s", esp_err_to_name(result));
        goto cleanup;
    }
    result = esp_wifi_init(&wifi_init_config);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init: %s", esp_err_to_name(result));
        goto cleanup;
    }
    wifi_initialized = true;
    result = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_storage: %s", esp_err_to_name(result));
        goto cleanup;
    }
    if (strlen(TRIAL_WIFI_SSID) == 0 || strlen(TRIAL_WIFI_SSID) > sizeof(wifi_config.sta.ssid) ||
        strlen(TRIAL_WIFI_PASSWORD) > sizeof(wifi_config.sta.password)) {
        ESP_LOGE(TAG, "Invalid private credential length; values omitted");
        goto cleanup;
    }
    memcpy(wifi_config.sta.ssid, TRIAL_WIFI_SSID, strlen(TRIAL_WIFI_SSID));
    memcpy(wifi_config.sta.password, TRIAL_WIFI_PASSWORD, strlen(TRIAL_WIFI_PASSWORD));

    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode: %s", esp_err_to_name(result));
        goto cleanup;
    }
    result = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config: %s", esp_err_to_name(result));
        goto cleanup;
    }

    wifi_wait_started_at = xTaskGetTickCount();
    result = esp_wifi_start();
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start: %s", esp_err_to_name(result));
        goto cleanup;
    }
    wifi_started = true;
    // Unsigned elapsed-tick subtraction tolerates one tick-counter wrap.
    wifi_wait_remaining = (TickType_t)(xTaskGetTickCount() - wifi_wait_started_at);
    if (wifi_wait_remaining >= wifi_wait_budget) {
        ESP_LOGE(TAG, "Wi-Fi startup deadline exhausted");
        goto cleanup;
    }
    wifi_wait_remaining = wifi_wait_budget - wifi_wait_remaining;
    xEventGroupWaitBits(wifi_events, WIFI_STARTED_BIT | WIFI_FAILED_BIT,
                        pdFALSE, pdFALSE, wifi_wait_remaining);
    wifi_bits = xEventGroupGetBits(wifi_events);
    if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_STARTED_BIT)) {
        ESP_LOGE(TAG, "Wi-Fi startup failure or timeout");
        goto cleanup;
    }
    wifi_wait_remaining = (TickType_t)(xTaskGetTickCount() - wifi_wait_started_at);
    if (wifi_wait_remaining >= wifi_wait_budget) {
        ESP_LOGE(TAG, "Wi-Fi connection deadline exhausted before connect");
        goto cleanup;
    }
    // Sole application-level connection attempt, after STA_START only.
    result = esp_wifi_connect();
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_connect: %s", esp_err_to_name(result));
        goto cleanup;
    }
    wifi_wait_remaining = (TickType_t)(xTaskGetTickCount() - wifi_wait_started_at);
    if (wifi_wait_remaining >= wifi_wait_budget) {
        ESP_LOGE(TAG, "Wi-Fi connection deadline exhausted");
        goto cleanup;
    }
    wifi_wait_remaining = wifi_wait_budget - wifi_wait_remaining;
    xEventGroupWaitBits(wifi_events, WIFI_READY_BIT | WIFI_FAILED_BIT,
                        pdFALSE, pdFALSE, wifi_wait_remaining);
    wifi_bits = xEventGroupGetBits(wifi_events);
    wifi_wait_remaining = (TickType_t)(xTaskGetTickCount() - wifi_wait_started_at);
    if (wifi_wait_remaining >= wifi_wait_budget || (wifi_bits & WIFI_FAILED_BIT) ||
        !(wifi_bits & WIFI_READY_BIT)) {
        ESP_LOGE(TAG, "Wi-Fi IPv4 failure or 30-second timeout");
        goto cleanup;
    }
    ESP_LOGI(TAG, "Internal heap after IPv4: free=%u bytes",
             (unsigned int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "socket: errno=%d", errno);
        goto cleanup;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout)) < 0) {
        ESP_LOGE(TAG, "setsockopt: errno=%d", errno);
        goto cleanup;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IPV4, &server_address.sin_addr) != 1) {
        ESP_LOGE(TAG, "Invalid server IPv4 configuration");
        goto cleanup;
    }
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        ESP_LOGE(TAG, "UDP connect: errno=%d", errno);
        goto cleanup;
    }
    if (getsockname(socket_fd, (struct sockaddr *)&local_address, &local_address_length) < 0) {
        ESP_LOGE(TAG, "getsockname: errno=%d", errno);
        goto cleanup;
    }
    ESP_LOGI(TAG, "UDP local=%s:%u peer=%s:%u", inet_ntoa(local_address.sin_addr),
             (unsigned int)ntohs(local_address.sin_port), SERVER_IPV4, (unsigned int)SERVER_PORT);

    while (attempted_exchanges < MAX_EXCHANGE_ATTEMPTS) {
        wifi_bits = xEventGroupGetBits(wifi_events);
        if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
            ESP_LOGE(TAG, "Wi-Fi unavailable before next send");
            break;
        }
        ++attempted_exchanges;
        ssize_t sent_length = send(socket_fd, PING_PAYLOAD, sizeof(PING_PAYLOAD), 0);
        if (sent_length != sizeof(PING_PAYLOAD)) {
            ESP_LOGE(TAG, "Exchange %u send failed: bytes=%d errno=%d",
                     attempted_exchanges, (int)sent_length, sent_length < 0 ? errno : 0);
            break;
        }
        ESP_LOGI(TAG, "Exchange %u sent PING: bytes=%d peer=%s:%u", attempted_exchanges,
                 (int)sent_length, SERVER_IPV4, (unsigned int)SERVER_PORT);
        ssize_t received_length = recv(socket_fd, receive_buffer, sizeof(receive_buffer), 0);
        if (received_length < 0) {
            ESP_LOGE(TAG, "Exchange %u receive %s: errno=%d", attempted_exchanges,
                     (errno == EAGAIN || errno == EWOULDBLOCK) ? "timeout" : "error", errno);
            break;
        }
        wifi_bits = xEventGroupGetBits(wifi_events);
        if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
            ESP_LOGE(TAG, "Exchange %u Wi-Fi failed before response validation", attempted_exchanges);
            break;
        }
        if (received_length != sizeof(PONG_PAYLOAD) ||
            memcmp(receive_buffer, PONG_PAYLOAD, sizeof(PONG_PAYLOAD)) != 0) {
            ESP_LOGE(TAG, "Exchange %u unexpected response: bytes=%d (expected exact PONG)",
                     attempted_exchanges, (int)received_length);
            break;
        }
        ++successful_exchanges;
        ESP_LOGI(TAG, "Exchange %u received exact PONG: bytes=%d successes=%u",
                 attempted_exchanges, (int)received_length, successful_exchanges);
        if (attempted_exchanges < MAX_EXCHANGE_ATTEMPTS) {
            vTaskDelay(pdMS_TO_TICKS(EXCHANGE_INTERVAL_MS));
        }
    }

cleanup:
    ESP_LOGI(TAG, "Result: attempts=%u successes=%u status=%s", attempted_exchanges,
             successful_exchanges, successful_exchanges == MAX_EXCHANGE_ATTEMPTS ? "PASS" : "FAIL");
    ESP_LOGI(TAG, "Internal heap: free=%u minimum=%u bytes; main stack minimum free=%u bytes",
             (unsigned int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned int)heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned int)uxTaskGetStackHighWaterMark(NULL));
    if (socket_fd >= 0 && close(socket_fd) < 0) {
        ESP_LOGW(TAG, "Socket cleanup failed: errno=%d", errno);
    }
    if (wifi_started) {
        result = esp_wifi_stop();
        if (result == ESP_OK) {
            wifi_started = false;
        } else {
            ESP_LOGW(TAG, "Wi-Fi stop failed: %s; retaining dependent resources", esp_err_to_name(result));
        }
    }
    if (wifi_handler_instance != NULL) {
        result = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_instance);
        if (result != ESP_OK) {
            event_group_delete_safe = false;
            ESP_LOGW(TAG, "Wi-Fi callback unregister failed: %s", esp_err_to_name(result));
        }
    }
    if (ip_handler_instance != NULL) {
        result = esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_handler_instance);
        if (result != ESP_OK) {
            event_group_delete_safe = false;
            ESP_LOGW(TAG, "IP callback unregister failed: %s", esp_err_to_name(result));
        }
    }
    if (wifi_events != NULL && event_group_delete_safe) {
        vEventGroupDelete(wifi_events);
        wifi_events = NULL;
    } else if (wifi_events != NULL) {
        ESP_LOGW(TAG, "Event group retained until reset for callback safety");
    }
    if (wifi_initialized && !wifi_started) {
        result = esp_wifi_deinit();
        if (result == ESP_OK) {
            wifi_initialized = false;
        } else {
            ESP_LOGW(TAG, "Wi-Fi deinit failed: %s; retaining dependent resources", esp_err_to_name(result));
        }
    }
    if (station_netif != NULL && !wifi_initialized && event_group_delete_safe) {
        esp_netif_destroy_default_wifi(station_netif);
        station_netif = NULL;
    }
    if (event_loop_created && !wifi_initialized && station_netif == NULL && event_group_delete_safe) {
        result = esp_event_loop_delete_default();
        if (result != ESP_OK) {
            ESP_LOGW(TAG, "Default event-loop deletion failed: %s", esp_err_to_name(result));
        }
    }
    if (nvs_initialized && !wifi_initialized) {
        result = nvs_flash_deinit();
        if (result != ESP_OK) {
            ESP_LOGW(TAG, "NVS deinit failed: %s", esp_err_to_name(result));
        }
    }
}
