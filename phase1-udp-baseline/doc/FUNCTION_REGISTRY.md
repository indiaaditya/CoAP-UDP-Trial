# Function and callback registry

Step 1 introduces no named function declarations or wrappers. Its four callbacks below are anonymous, in phase1-udp-baseline/src/cloud.js, introduced in Phase 1, Step 1. Return values are unused (undefined).

| Callback | Purpose | Inputs | Outputs / effects | Trigger |
| --- | --- | --- | --- | --- |
| listening | Report bound endpoint | None; reads socket.address() | Console listening line | Socket listening event |
| message | Log packet; recognize exact PING | message: Buffer; rinfo: sender metadata | Reception log; send PONG to sender on match | Socket message event |
| send completion | Report completion or send error | error: Error or null | Destination line or error details; no retry | socket.send completion |
| error | Report bind/runtime error and terminate | error: Error | Error code/message; exitCode=1; socket.close() | Socket error event |

Successful send completion does not establish delivery. Client observations establish receipt. Ctrl+C uses default Node.js termination without a registered shutdown callback.


## Phase 1, Step 2 — implemented firmware

File: phase1-udp-baseline/src/esp32-udp-client/main/main.c. No custom tasks, wrappers, macros that wrap calls, or additional function declarations. No new NodeJS callbacks.

| Custom function | Return / exact parameters | Purpose / caller | Success | Failure |
| --- | --- | --- | --- | --- |
| app_main | void; void | SDK entry; initialization, one connection, bounded UDP experiment, measurement logs, shared cleanup | At most 20 attempted exchanges; 20 exact PONG responses yield PASS; cleanup and return | Log failed stage and partial counts, no retry; ownership-aware cleanup and return |
| wifi_event_handler (static) | void; void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data | SDK event loop callback for WIFI_EVENT/ESP_EVENT_ANY_ID and IP_EVENT/IP_EVENT_STA_GOT_IP | STA_START sets STARTED; GOT_IP logs IPv4 and sets READY if FAILED clear | Disconnect logs reason, clears READY, latches FAILED; never calls connect or sockets |

### SDK/library calls and ownership

- app_main is the sole caller of esp_wifi_connect, exactly once after the STARTED event and only within the remaining shared 30-second budget.
- ESP-IDF: nvs_flash_init/deinit; esp_netif_init/create_default_wifi_sta/destroy_default_wifi; esp_event_loop_create_default/delete_default; esp_event_handler_instance_register/unregister; esp_wifi_init/set_storage/set_mode/set_config/start/connect/stop/deinit; esp_err_to_name; ESP_LOGI/ESP_LOGE/ESP_LOGW.
- FreeRTOS: xEventGroupCreate/WaitBits/GetBits/SetBits/ClearBits, vEventGroupDelete, xTaskGetTickCount, vTaskDelay, uxTaskGetStackHighWaterMark, pdMS_TO_TICKS. Callback uses only logging and event-group Get/Set/Clear operations.
- lwIP BSD sockets: socket, setsockopt, inet_pton, connect (UDP peer selection, no handshake), getsockname, send, recv, close; htons/ntohs and inet_ntoa for endpoint conversion/logging.
- C library: strlen, memcpy, memcmp, errno.
- Heap observation: heap_caps_get_free_size and heap_caps_get_minimum_free_size with MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT.

### Headers and built-in component dependencies

Standard headers: stdint.h, stdbool.h, stddef.h, string.h, errno.h, sys/socket.h, sys/time.h, unistd.h. SDK headers: lwip/inet.h; freertos/FreeRTOS.h, task.h, event_groups.h; esp_event.h, esp_netif.h, esp_wifi.h, esp_wifi_default.h, esp_log.h, esp_err.h, esp_heap_caps.h, nvs_flash.h. Private header: wifi_credentials.h (local input only).

Explicit component dependencies: esp_wifi, esp_event, esp_netif, nvs_flash, lwip, freertos, log, esp_common, heap. All are ESP-IDF-provided; no external application dependency. Standard SDK transitive platform dependencies remain managed by ESP-IDF. Wi-Fi and TCP/IP use SDK tasks; application code creates none.

### Cleanup safety

Close socket; stop Wi-Fi if started; unregister both application handler instances using exact registration keys. Only after successful unregistration of all registered application callbacks is the event group deleted and its static handle cleared. If unregistration fails, retain the group until reset. Continue Wi-Fi deinit, default interface destruction, default loop deletion and NVS deinit only where ownership/dependency guards permit. Cleanup errors are separately logged; dependent objects may remain until reset. ESP-NETIF/lwIP infrastructure remains until reset.

Callback safety is based on inspected ESP-IDF 5.5.4 components/esp_event/esp_event.c: dispatch invokes callbacks under the loop mutex; unregister synchronizes with that mutex and removes or marks registrations unregistered before returning. Cleanup runs in app_main, never from a callback.


## Step 3 audit — 2026-11-11

Checked against committed cloud.js and main/main.c at b13620f: exactly four significant anonymous NodeJS callbacks and two custom C function definitions; their names, signatures, triggers, API ownership and cleanup responsibilities match this registry. No additional custom functions/tasks or external dependencies were found. The sole esp_wifi_connect call is in app_main after the STARTED wait. The final approved G2 flow and cross-references are consolidated in project-doc/ARCHITECTURE_DECISIONS.md. This was static documentation/source inspection only, not another hardware test or fault-injection run.
