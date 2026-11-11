# Variable and constant registry

All are module-scoped const bindings in phase1-udp-baseline/src/cloud.js, introduced in Phase 1, Step 1. No persistent application state is introduced.

| Name | Type | Value / purpose |
| --- | --- | --- |
| dgram | Built-in module object | node:dgram API |
| UDP_PORT | number | 56830; edit to configure the listening port |
| BIND_ADDRESS | string | 0.0.0.0; all local IPv4 interfaces |
| PING_PAYLOAD | Buffer | ASCII PING, hex 50 49 4e 47; exact incoming match |
| PONG_PAYLOAD | Buffer | ASCII PONG, hex 50 4f 4e 47; response |
| socket | dgram.Socket | udp4 reception and transmission socket |

Routine callback-local address, message, rinfo and error values are described in the function registry and are not retained state.


## Phase 1, Step 2 — actual firmware variables

Owner/file: phase1-udp-baseline/src/esp32-udp-client/main/main.c. All introduced in Step 2. Routine API results and callback casts are documented below without creating additional application state.

| Name | Exact C type | Scope / storage / lifetime | Initial value / purpose |
| --- | --- | --- | --- |
| TAG | static const char[] | File / static / boot | udp_trial log tag |
| SERVER_IPV4 | static const char[] | File / static / boot | 192.168.0.170 |
| SERVER_PORT | static const uint16_t | File / static / boot | 56830 |
| PING_PAYLOAD | static const uint8_t[4] | File / static / boot | Four ASCII request bytes, no terminator |
| PONG_PAYLOAD | static const uint8_t[4] | File / static / boot | Four expected response bytes |
| MAX_EXCHANGE_ATTEMPTS | static const unsigned int | File / static / boot | 20; count attempts, not only successes |
| WIFI_CONNECT_TIMEOUT_MS | static const uint32_t | File / static / boot | 30000; shared startup/IPv4 budget |
| RECEIVE_TIMEOUT_SECONDS | static const long | File / static / boot | 2 |
| EXCHANGE_INTERVAL_MS | static const uint32_t | File / static / boot | 1000; only between successful exchanges |
| WIFI_READY_BIT | static const EventBits_t | File / static / boot | BIT0 |
| WIFI_FAILED_BIT | static const EventBits_t | File / static / boot | BIT1; latched until end of run |
| WIFI_STARTED_BIT | static const EventBits_t | File / static / boot | BIT2 |
| wifi_events | static EventGroupHandle_t | File / static handle; group allocated once | NULL; delete only after successful callback unregistration, otherwise retain until reset |
| wifi_init_config | wifi_init_config_t | app_main / automatic / main run | WIFI_INIT_CONFIG_DEFAULT() |
| wifi_config | wifi_config_t | app_main / automatic / main run | Zero; private credentials copied without truncation |
| station_netif | esp_netif_t * | app_main / automatic handle / main run | NULL; default station interface |
| wifi_handler_instance | esp_event_handler_instance_t | app_main / automatic handle / main run | NULL; WIFI_EVENT/ANY_ID registration |
| ip_handler_instance | esp_event_handler_instance_t | app_main / automatic handle / main run | NULL; IP_EVENT/GOT_IP registration |
| nvs_initialized | bool | app_main / automatic / main run | false; cleanup ownership |
| event_loop_created | bool | app_main / automatic / main run | false; cleanup ownership |
| wifi_initialized | bool | app_main / automatic / main run | false; cleanup ownership |
| wifi_started | bool | app_main / automatic / main run | false; safe stop/deinit guard |
| event_group_delete_safe | bool | app_main / automatic / main run | true; false on any handler-unregistration failure |
| socket_fd | int | app_main / automatic / main run | -1; UDP socket handle |
| server_address | struct sockaddr_in | app_main / automatic / main run | Zero; configured peer |
| local_address | struct sockaddr_in | app_main / automatic / main run | Zero; getsockname result |
| local_address_length | socklen_t | app_main / automatic / main run | sizeof(local_address) |
| receive_timeout | struct timeval | app_main / automatic / main run | {2, 0} |
| receive_buffer | uint8_t[64] | app_main / automatic / main run | Receive scratch buffer; compare bytes only after length check |
| attempted_exchanges | unsigned int | app_main / automatic / main run | 0; increment immediately before send, including failed sends |
| successful_exchanges | unsigned int | app_main / automatic / main run | 0; increment only for exact PONG and valid Wi-Fi bits |
| sent_length | ssize_t | Exchange block / automatic / iteration | Return from send |
| received_length | ssize_t | Exchange block / automatic / iteration | Return from recv |
| wifi_bits | EventBits_t | app_main / automatic / main run | Current bit snapshot |
| wifi_wait_started_at | TickType_t | app_main / automatic / main run | xTaskGetTickCount immediately before esp_wifi_start |
| wifi_wait_budget | const TickType_t | app_main / automatic / main run | pdMS_TO_TICKS(30000) |
| wifi_wait_remaining | TickType_t | app_main / automatic / main run | Unsigned elapsed ticks, then budget minus elapsed after exhaustion check |
| result | esp_err_t | app_main / automatic / main run | Routine SDK API return code; never read before assignment |
| event | const wifi_event_sta_disconnected_t * or const ip_event_got_ip_t * | Callback branch / automatic / invocation | Borrowed event_data; not retained |

Private macros TRIAL_WIFI_SSID and TRIAL_WIFI_PASSWORD are string literals defined only in the ignored .private/wifi_credentials.h. No values recorded here. They compile into local firmware, so generated outputs are not tracked.

Deadline arithmetic uses (TickType_t)(now - start), compares elapsed against budget before subtraction, and waits only on the remaining budget. Both startup and IPv4 waits share the same deadline; the tick count may wrap once safely during the short bounded interval. No new application task, retry counter, persistent device state or protocol field.
