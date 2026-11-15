#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
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
#include "coap3/coap.h"
#include "wifi_credentials.h"

static const char TAG[] = "coap_trial";
static const char SERVER_IPV4[] = "192.168.0.170";
static const uint16_t SERVER_PORT = 56830;
static const uint8_t PING_PAYLOAD[4] = {'P', 'I', 'N', 'G'};
static const uint8_t PONG_PAYLOAD[4] = {'P', 'O', 'N', 'G'};
static const unsigned int MAX_EXCHANGE_ATTEMPTS = 20;
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 30000;
static const uint32_t TRANSACTION_TIMEOUT_MS = 100000;
static const uint32_t IO_SLICE_MS = 100;
static const coap_fixed_point_t ACK_TIMEOUT = {2, 0};
static const coap_fixed_point_t ACK_RANDOM_FACTOR = {1, 500};
static const uint16_t MAX_RETRANSMIT = 4;
static const uint16_t NSTART = 1;
static const size_t TOKEN_LENGTH = 4;
static const uint32_t HEAP_CAPS = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
static const uint8_t RESOURCE_PATH[] = "coap-test";
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

typedef enum {
    TRIAL_IDLE, TRIAL_PENDING, TRIAL_SUCCEEDED, TRIAL_FAILED
} trial_state_t;

typedef enum {
    TRIAL_ERROR_NONE, TRIAL_ERROR_BUILD, TRIAL_ERROR_SEND, TRIAL_ERROR_NACK,
    TRIAL_ERROR_IO, TRIAL_ERROR_DEADLINE, TRIAL_ERROR_RESPONSE,
    TRIAL_ERROR_WIFI, TRIAL_ERROR_MEASUREMENT
} trial_error_t;

typedef struct {
    trial_state_t state;
    trial_error_t error;
    uint8_t token[8];
    size_t token_length;
    coap_mid_t request_mid;
    TickType_t started_at;
    coap_nack_reason_t nack_reason; /* Read only for TRIAL_ERROR_NACK. */
} trial_transaction_t;

static bool send_ping_request(coap_session_t *session, trial_transaction_t *transaction)
{
    coap_pdu_t *pdu = NULL;
    transaction->state = TRIAL_PENDING;
    transaction->error = TRIAL_ERROR_NONE;
    transaction->request_mid = COAP_INVALID_MID;
    coap_session_new_token(session, &transaction->token_length, transaction->token);
    if (transaction->token_length != TOKEN_LENGTH) {
        goto construction_failed;
    }
    pdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST, session);
    if (pdu == NULL || !coap_add_token(pdu, transaction->token_length, transaction->token) ||
        coap_add_option(pdu, COAP_OPTION_URI_PATH, sizeof(RESOURCE_PATH) - 1,
                        RESOURCE_PATH) == 0 ||
        coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT, 0, NULL) == 0 ||
        !coap_add_data(pdu, sizeof(PING_PAYLOAD), PING_PAYLOAD)) {
        goto construction_failed;
    }
    transaction->started_at = xTaskGetTickCount();
    /* coap_send owns pdu, even if it returns COAP_INVALID_MID. */
    transaction->request_mid = coap_send(session, pdu);
    pdu = NULL;
    if (transaction->request_mid == COAP_INVALID_MID) {
        if (transaction->error == TRIAL_ERROR_NONE) {
            transaction->error = TRIAL_ERROR_SEND;
        }
        transaction->state = TRIAL_FAILED;
        return false;
    }
    return transaction->state != TRIAL_FAILED;

construction_failed:
    if (pdu != NULL) {
        coap_delete_pdu(pdu);
    }
    transaction->error = TRIAL_ERROR_BUILD;
    transaction->state = TRIAL_FAILED;
    return false;
}

static coap_response_t coap_response_handler(coap_session_t *session,
                                            const coap_pdu_t *sent,
                                            const coap_pdu_t *received,
                                            const coap_mid_t mid)
{
    (void)sent; /* Can be NULL for a separate response. */
    trial_transaction_t *transaction = coap_session_get_app_data(session);
    if (transaction == NULL) {
        return COAP_RESPONSE_FAIL;
    }
    coap_bin_const_t token = coap_pdu_get_token(received);
    if (token.length != transaction->token_length ||
        memcmp(token.s, transaction->token, token.length) != 0) {
        ESP_LOGW(TAG, "Rejected unrelated response: mid=%d token_length=%u",
                 (int)mid, (unsigned int)token.length);
        return COAP_RESPONSE_FAIL;
    }
    if (transaction->state == TRIAL_SUCCEEDED) {
        /* ACK a recognized duplicate without counting success again. */
        return COAP_RESPONSE_OK;
    }
    if (transaction->state != TRIAL_PENDING) {
        return COAP_RESPONSE_FAIL;
    }
    if ((TickType_t)(xTaskGetTickCount() - transaction->started_at) >=
        pdMS_TO_TICKS(TRANSACTION_TIMEOUT_MS)) {
        transaction->error = TRIAL_ERROR_DEADLINE;
        transaction->state = TRIAL_FAILED;
        return COAP_RESPONSE_FAIL;
    }
    EventBits_t wifi_bits = xEventGroupGetBits(wifi_events);
    if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
        transaction->error = TRIAL_ERROR_WIFI;
        transaction->state = TRIAL_FAILED;
        return COAP_RESPONSE_FAIL;
    }
    coap_pdu_type_t type = coap_pdu_get_type(received);
    coap_opt_iterator_t iterator;
    coap_opt_t *content_format = coap_check_option(received, COAP_OPTION_CONTENT_FORMAT, &iterator);
    size_t length = 0;
    const uint8_t *data = NULL;
    if ((type != COAP_MESSAGE_ACK && type != COAP_MESSAGE_CON && type != COAP_MESSAGE_NON) ||
        (type == COAP_MESSAGE_ACK && coap_pdu_get_mid(received) != transaction->request_mid) ||
        coap_pdu_get_code(received) != COAP_RESPONSE_CODE_CHANGED ||
        content_format == NULL || coap_opt_length(content_format) > 2 ||
        coap_decode_var_bytes(coap_opt_value(content_format), coap_opt_length(content_format)) != 0 ||
        coap_check_option(received, COAP_OPTION_OBSERVE, &iterator) != NULL ||
        coap_check_option(received, COAP_OPTION_BLOCK1, &iterator) != NULL ||
        coap_check_option(received, COAP_OPTION_BLOCK2, &iterator) != NULL ||
        !coap_get_data(received, &length, &data) || length != sizeof(PONG_PAYLOAD) ||
        memcmp(data, PONG_PAYLOAD, sizeof(PONG_PAYLOAD)) != 0) {
        transaction->error = TRIAL_ERROR_RESPONSE;
        transaction->state = TRIAL_FAILED;
        ESP_LOGE(TAG, "Invalid matching response: mid=%d code=%u bytes=%u",
                 (int)mid, (unsigned int)coap_pdu_get_code(received), (unsigned int)length);
        /* Intentional RST for rejected CON/NON, not for an ACK response. */
        return COAP_RESPONSE_FAIL;
    }
    transaction->state = TRIAL_SUCCEEDED;
    /* libcoap sends a CON-response ACK after this callback returns, before
       the enclosing coap_io_process returns. Never clean up here. */
    return COAP_RESPONSE_OK;
}

static void coap_nack_handler(coap_session_t *session, const coap_pdu_t *sent,
                              const coap_nack_reason_t reason, const coap_mid_t mid)
{
    (void)sent;
    trial_transaction_t *transaction = coap_session_get_app_data(session);
    if (transaction != NULL && transaction->state == TRIAL_PENDING &&
        (transaction->request_mid == COAP_INVALID_MID || transaction->request_mid == mid)) {
        transaction->nack_reason = reason;
        transaction->error = TRIAL_ERROR_NACK;
        transaction->state = TRIAL_FAILED;
        ESP_LOGE(TAG, "CoAP NACK: mid=%d reason=%d", (int)mid, (int)reason);
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
    bool coap_started = false;
    coap_context_t *context = NULL;
    coap_session_t *session = NULL;
    coap_address_t server_address;
    uint8_t token_seed[4];
    trial_transaction_t transaction = {
        .state = TRIAL_IDLE, .error = TRIAL_ERROR_NONE, .token = {0},
        .token_length = 0, .request_mid = COAP_INVALID_MID, .started_at = 0
    };
    unsigned int attempted_exchanges = 0;
    unsigned int successful_exchanges = 0;
    EventBits_t wifi_bits;
    TickType_t wifi_wait_started_at;
    const TickType_t wifi_wait_budget = pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS);
    TickType_t wifi_wait_remaining;
    TickType_t pacing_started_at;
    const TickType_t transaction_budget = pdMS_TO_TICKS(TRANSACTION_TIMEOUT_MS);
    const TickType_t pacing_budget = pdMS_TO_TICKS(EXCHANGE_INTERVAL_MS);
    esp_err_t result;

    ESP_LOGI(TAG, "ESP32-S3 CoAP trial: peer=%s:%u maximum_attempts=%u",
             SERVER_IPV4, (unsigned int)SERVER_PORT, MAX_EXCHANGE_ATTEMPTS);
    ESP_LOGI(TAG, "B1 internal heap before Wi-Fi: free=%u bytes",
             (unsigned int)heap_caps_get_free_size(HEAP_CAPS));

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
    ESP_LOGI(TAG, "B2 internal heap after IPv4: free=%u bytes",
             (unsigned int)heap_caps_get_free_size(HEAP_CAPS));

    coap_startup();
    coap_started = true;
    context = coap_new_context(NULL);
    if (context == NULL) {
        ESP_LOGE(TAG, "CoAP context allocation failed");
        goto cleanup;
    }
    coap_context_set_block_mode(context, 0);
    coap_context_set_max_token_size(context, 8);
    coap_register_response_handler(context, coap_response_handler);
    coap_register_nack_handler(context, coap_nack_handler);
    coap_address_init(&server_address);
    server_address.addr.sin.sin_family = AF_INET;
    server_address.addr.sin.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IPV4, &server_address.addr.sin.sin_addr) != 1) {
        ESP_LOGE(TAG, "Invalid server IPv4 configuration");
        goto cleanup;
    }
    session = coap_new_client_session(context, NULL, &server_address, COAP_PROTO_UDP);
    if (session == NULL) {
        ESP_LOGE(TAG, "CoAP session allocation failed");
        goto cleanup;
    }
    coap_session_set_app_data(session, &transaction);
    coap_session_set_ack_timeout(session, ACK_TIMEOUT);
    coap_session_set_ack_random_factor(session, ACK_RANDOM_FACTOR);
    coap_session_set_max_retransmit(session, MAX_RETRANSMIT);
    coap_session_set_nstart(session, NSTART);
    if (!coap_prng(token_seed, sizeof(token_seed))) {
        ESP_LOGE(TAG, "CoAP token seed generation failed");
        goto cleanup;
    }
    token_seed[0] = 0x80 | (token_seed[0] & 0x3f);
    coap_session_init_token(session, sizeof(token_seed), token_seed);
    ESP_LOGI(TAG, "B3 internal heap before first exchange: free=%u bytes",
             (unsigned int)heap_caps_get_free_size(HEAP_CAPS));

    while (attempted_exchanges < MAX_EXCHANGE_ATTEMPTS) {
        wifi_bits = xEventGroupGetBits(wifi_events);
        if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
            transaction.error = TRIAL_ERROR_WIFI;
            transaction.state = TRIAL_FAILED;
            break;
        }
        ++attempted_exchanges;
        if (!send_ping_request(session, &transaction)) {
            ESP_LOGE(TAG, "Exchange %u submission failed: error=%d",
                     attempted_exchanges, (int)transaction.error);
            break;
        }
        ESP_LOGI(TAG, "Exchange %u POST /coap-test PING: mid=%d token=%02x%02x%02x%02x",
                 attempted_exchanges, (int)transaction.request_mid, transaction.token[0],
                 transaction.token[1], transaction.token[2], transaction.token[3]);
        while (transaction.state == TRIAL_PENDING) {
            TickType_t elapsed = (TickType_t)(xTaskGetTickCount() - transaction.started_at);
            if (elapsed >= transaction_budget) {
                transaction.error = TRIAL_ERROR_DEADLINE;
                transaction.state = TRIAL_FAILED;
                break;
            }
            uint32_t wait_ms = (uint32_t)(transaction_budget - elapsed) * portTICK_PERIOD_MS;
            if (wait_ms > IO_SLICE_MS) {
                wait_ms = IO_SLICE_MS;
            }
            if (coap_io_process(context, wait_ms) < 0) {
                transaction.error = TRIAL_ERROR_IO;
                transaction.state = TRIAL_FAILED;
                break;
            }
            wifi_bits = xEventGroupGetBits(wifi_events);
            if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
                transaction.error = TRIAL_ERROR_WIFI;
                transaction.state = TRIAL_FAILED;
            }
        }
        if (transaction.state != TRIAL_SUCCEEDED) {
            ESP_LOGE(TAG, "Exchange %u failed: error=%d", attempted_exchanges,
                     (int)transaction.error);
            break;
        }
        /* The I/O call has returned after the callback and libcoap ACK path,
           including for the final separate CON response. */
        ++successful_exchanges;
        ESP_LOGI(TAG, "Exchange %u received exact PONG: successes=%u",
                 attempted_exchanges, successful_exchanges);
        if (attempted_exchanges < MAX_EXCHANGE_ATTEMPTS) {
            pacing_started_at = xTaskGetTickCount();
            while (true) {
                TickType_t elapsed = (TickType_t)(xTaskGetTickCount() - pacing_started_at);
                if (elapsed >= pacing_budget) {
                    break;
                }
                TickType_t remaining = pacing_budget - elapsed;
                uint32_t wait_ms = (uint32_t)remaining * portTICK_PERIOD_MS;
                if (wait_ms > IO_SLICE_MS) {
                    wait_ms = IO_SLICE_MS;
                }
                if (coap_io_process(context, wait_ms) < 0) {
                    transaction.error = TRIAL_ERROR_IO;
                    transaction.state = TRIAL_FAILED;
                    break;
                }
                wifi_bits = xEventGroupGetBits(wifi_events);
                if ((wifi_bits & WIFI_FAILED_BIT) || !(wifi_bits & WIFI_READY_BIT)) {
                    transaction.error = TRIAL_ERROR_WIFI;
                    transaction.state = TRIAL_FAILED;
                    break;
                }
            }
            if (transaction.state == TRIAL_FAILED) {
                break;
            }
        }
    }

cleanup:
    ESP_LOGI(TAG, "Result: attempts=%u successes=%u status=%s", attempted_exchanges,
             successful_exchanges, successful_exchanges == MAX_EXCHANGE_ATTEMPTS ? "PASS" : "FAIL");
    ESP_LOGI(TAG, "B5 internal heap free=%u; whole-run minimum=%u; B6 main stack minimum free=%u bytes",
             (unsigned int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned int)heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned int)uxTaskGetStackHighWaterMark(NULL));
    if (session != NULL) {
        coap_session_set_app_data(session, NULL);
        coap_session_release(session);
    }
    if (context != NULL) {
        coap_free_context(context);
    }
    if (coap_started) {
        coap_cleanup();
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
