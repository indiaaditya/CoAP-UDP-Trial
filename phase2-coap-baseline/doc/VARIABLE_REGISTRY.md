# Phase 2 variable registry — G3-A actual implementation

All C state is in src/esp32-coap-client/main/main.c. No global transaction or application-created task. Routine local API results, callback borrowed pointers and option iterators are not persistent application state.

## File-scope constants/handle (exact declarations)

```c
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
```

Constants have static process/boot lifetime. wifi_events starts NULL and is owned by app_main; Wi-Fi callback accesses it through guarded registration lifetime.

## Transaction types and ownership

trial_state_t: TRIAL_IDLE, TRIAL_PENDING, TRIAL_SUCCEEDED, TRIAL_FAILED.
trial_error_t: TRIAL_ERROR_NONE, BUILD, SEND, NACK, IO, DEADLINE, RESPONSE, WIFI, MEASUREMENT with full TRIAL_ERROR_ prefixes. MEASUREMENT is reserved for approved G3-B; not yet used.

| trial_transaction_t field | Exact type | Default / owner |
| --- | --- | --- |
| state | trial_state_t | IDLE; main and synchronous CoAP callbacks |
| error | trial_error_t | NONE |
| token | uint8_t[8] | zeros; API requires capacity 8, actual transmitted length 4 |
| token_length | size_t | 0, then library output checked as 4 |
| request_mid | coap_mid_t | COAP_INVALID_MID |
| started_at | TickType_t | 0, set immediately before send |
| nack_reason | coap_nack_reason_t | zero-initialized; read only after NACK |

transaction is automatic storage in app_main for its entire run. Session borrows &transaction, cleared before release. All CoAP access is in main task; no mutex/volatile. wifi handler touches only event bits/logs.

## app_main significant automatic variables

| Names | Exact type | Initialization / purpose |
| --- | --- | --- |
| wifi_init_config | wifi_init_config_t | WIFI_INIT_CONFIG_DEFAULT() |
| wifi_config | wifi_config_t | zero; private credential length validation/copy |
| station_netif | esp_netif_t * | NULL; station interface |
| wifi_handler_instance, ip_handler_instance | esp_event_handler_instance_t each | NULL; handler ownership |
| nvs_initialized, event_loop_created, wifi_initialized, wifi_started | bool each | false; cleanup guards |
| event_group_delete_safe | bool | true; unregistration safety |
| coap_started | bool | false; library cleanup ownership |
| context | coap_context_t * | NULL; main-owned |
| session | coap_session_t * | NULL; main-owned reference |
| server_address | coap_address_t | coap_address_init; explicit IPv4/port |
| token_seed | uint8_t[4] | library PRNG; first byte normalized 0x80..0xbf |
| transaction | trial_transaction_t | initialized as above |
| attempted_exchanges, successful_exchanges | unsigned int each | 0; logical attempts/successes, not datagrams |
| wifi_bits | EventBits_t | API snapshot |
| wifi_wait_started_at, wifi_wait_remaining, pacing_started_at | TickType_t each | assigned before use |
| wifi_wait_budget, transaction_budget, pacing_budget | const TickType_t each | pdMS_TO_TICKS(30000/100000/1000) |
| result | esp_err_t | routine SDK return |

send_ping_request local pdu is coap_pdu_t *, NULL initially; owned until coap_send, which consumes even on failure. Callback token is coap_bin_const_t; received PDU/data/options are borrowed. Runtime heap readings are logged directly in G3-A; full measurement storage/count/monitor fields belong to deferred G3-B.

## Node module

coap: module object; COAP_PORT: number, default 56830 or validated TRIAL_COAP_PORT; BIND_ADDRESS: string 0.0.0.0; RESOURCE_PATH: string /coap-test; PING_PAYLOAD/PONG_PAYLOAD: Buffer, four bytes; SERVER_OPTIONS: object (udp4, proxy false, piggybackReplyMs 50, sendAcksForNonConfirmablePackets false, cacheSize 65536); server: package server; server_failed: boolean initially false. Process lifetime. Callback req/res/error are borrowed invocation arguments; no custom persistent device state.
