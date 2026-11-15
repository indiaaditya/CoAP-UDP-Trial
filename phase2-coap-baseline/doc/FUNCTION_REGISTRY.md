# Phase 2 function registry — G3-A

G2 approved; only G3-A implemented. No custom tasks or library adapter. All five C functions are in src/esp32-coap-client/main/main.c; all four JS functions are in src/cloud.js. Signatures below are actual definitions (JavaScript parameter contracts are documentation).

| Function | Signature | Caller / purpose | Major APIs | Success / failure |
| --- | --- | --- | --- | --- |
| app_main | void app_main(void) | ESP-IDF entry; network, CoAP, bounded campaign, essential measurements, cleanup | nvs_flash, esp_netif, esp_event, esp_wifi, FreeRTOS events/ticks, coap_startup/new_context/new_client_session/io_process/release/free_context/cleanup, heap/stack | 20/20 valid transactions -> communication PASS; setup/error stops with partial counts; guarded cleanup |
| wifi_event_handler | static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) | SDK default loop; STARTED/READY/FAILED | xEventGroup Get/Set/ClearBits; logging | READY only if no FAILED; disconnect latches FAILED; no connect or CoAP call |
| send_ping_request | static bool send_ping_request(coap_session_t *session, trial_transaction_t *transaction) | app_main; build/submit one logical CON POST | session_new_token, new_pdu, add_token/option/data, send, delete_pdu | true if submitted without synchronous terminal failure; false with BUILD/SEND/NACK; unsent PDU deleted, submitted PDU never accessed/deleted |
| coap_response_handler | static coap_response_t coap_response_handler(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, const coap_mid_t mid) | libcoap, main-task call stack; validate response | session_get_app_data; PDU/token/options/data accessors; event bits; memcmp | matching valid response sets SUCCEEDED, returns OK; duplicate recognized without recount; matching invalid response fails, returns FAIL; unrelated token never succeeds |
| coap_nack_handler | static void coap_nack_handler(coap_session_t *session, const coap_pdu_t *sent, const coap_nack_reason_t reason, const coap_mid_t mid) | libcoap; pending request failure | session_get_app_data; logging | matching pending request -> FAILED/NACK; irrelevant callbacks ignored |
| onRequest | function onRequest(req, res) -> undefined | Node package request event | public req fields/headers/options; res.on/setOption/end | POST /coap-test, text/plain, PING -> 2.04/PONG; wrong path 4.04; wrong method 4.05; invalid body/format/excluded options 4.00 |
| onListening | function onListening(error) -> undefined | server.listen callback | console; onServerError | endpoint log or failure delegation |
| onServerError | function onServerError(error) -> undefined | server error/listen error | console; process.exitCode; server.close | exitCode 1; one close attempt |
| onResponseError | function onResponseError(error) -> undefined | response error event | console.error | log only; no restart/retry |

Framework functions are not custom functions. libcoap owns serialization, socket I/O, MID generation, request retransmission, ACK/RST processing and duplicate handling. SDK owns Wi-Fi/event/TCP-IP tasks. No additional callback, wrapper or application task was added.

Callback return is separate from application state: FAIL intentionally requests RST for rejected CON/NON; not for ACK responses. Empty ACK does not complete a transaction. Final CON response ACK path executes after callback return and before enclosing coap_io_process returns; cleanup occurs only afterward. ACK transmission/delivery is unverified until G4.

sent can be NULL; callback PDU/data/options are borrowed and not retained. Session app data borrows main's transaction until cleared before release.
