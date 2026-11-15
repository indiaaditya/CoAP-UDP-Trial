# Phase 2 function registry — G3-A

G2 approved; G3-A accepted at aad119d4a980b37e510940e37a85b835a1630717; G3-B accepted at c146cb2353feb5cc467f85968fb9f6546fb1f607; G3-C fixture candidate implemented. No custom tasks or library adapter. All five C functions are in src/esp32-coap-client/main/main.c; all four JS functions are in src/cloud.js. Signatures below are actual definitions (JavaScript parameter contracts are documentation).

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


## G3-B scope update

G3-A accepted at aad119d4a980b37e510940e37a85b835a1630717. All five C signatures and four Node callbacks unchanged. Only app_main adds approved checkpoint storage, task-count snapshots and conditional SDK monitor lifecycle/status. The other four C functions, the exchange/pacing loops, CoAP teardown and Node sources remain unchanged.

Added SDK calls: uxTaskGetNumberOfTasks, heap_caps_monitor_local_minimum_free_size_start/stop. Primary excludes monitor calls at compile time; scoped reads B4 before stop and omits whole-run minimum reporting. On monitor start failure, log measurement failure and stop before sending; on stop failure, label measurement status FAIL and continue guarded resource cleanup. No custom helper added. B1/B2/B3 are logged only when reached; B4 is explicitly unavailable when monitoring was not started; B5/B6 are result-stage pre-CoAP-cleanup readings also on abort, not a fabricated completed campaign.


## G3-C host fixture - exact custom functions

Owner: test/coap_fixture.js. No additional server or ESP32 function. JavaScript contracts below document parameter types; source remains plain JavaScript. All functions return undefined. dgram sockets and codec functions are Node/package APIs, not custom functions.

| Custom function | Parameter contract | Caller / purpose / major APIs | Success / failure |
| --- | --- | --- | --- |
| main | () | Module entry; validate mode/CASE, create/bind dgram sockets, register named callbacks and watchdog | Starts selected mode/probe; invalid input/start error -> finish(false) |
| onFrontMessage | (message: Buffer, rinfo: RemoteInfo) | Front message event; coapPacket.parse/generate; forwarding, synthetic response/probe validation, timers | Logs/forwards/injects expected scenario; ignores unexpected peer; invalid input/probe -> finish(false) |
| onBackMessage | (message: Buffer, rinfo: RemoteInfo) | Backend message event; source validation, codec.parse, response drop/duplication | Forwards unchanged response to locked client; source mismatch ignored; parse/limit error fails |
| sendDatagram | (socket: dgram.Socket, message: Buffer, port: number, address: string) | Handlers/timer; bounded capture and socket.send(..., onSendComplete) | Queue datagram; synchronous error/limit -> finish(false); malformed outgoing injection is intentional |
| sendScheduledDatagram | (socket: dgram.Socket, message: Buffer, port: number, address: string) | setTimeout callback | Calls sendDatagram unless ended |
| onSendComplete | (error: Error or null, bytes: number) | Socket send callback | Completion log, not delivery proof; probe separate-CON success waits for four-byte ACK send completion; errors fail |
| onSocketError | (error: Error) | Socket error event | finish(false), close owned sockets |
| onListening | () with socket this | Socket listening event; address(), codec.generate | Endpoint log; probe sends once when front ready |
| onTimeout | () | 180000 ms host watchdog | finish(false, timeout) |
| onSignal | () | SIGINT/SIGTERM | finish(true, operator-stop); explicitly not a test PASS |
| finish | (passed: boolean, reason: string) | Terminal paths | Idempotent; clear watchdog/scheduled timers, remove signal handlers, close sockets, set exit code, final summary; non-benign close errors set exit1 |

Runtime package resolution intentionally uses ../src/node_modules/coap-packet because the approved npm manifest is under src while the fixture is under test. This loads the package public entry point, not coap internals. No new module helper/framework or production server change.

Synthetic peers are scoped test stimuli, not a second production CoAP implementation. In particular, separate modes do not implement a general server retransmission engine; loss tests use the unchanged real Node server via proxy mode at G4. No host socket was opened during G3-C validation.


## Watchdog-only amendment validation

Accepted G3-C source7dbe6f2298fbdcbeee34669c097ca55648b22f71. Exact11 fixture functions and16 modes unchanged; only constant value120000 ->180000.27 applicable mocked checks passed, including old120s boundary remaining active, timeout at180s,93s/100s observation windows with first arrival60s, unchanged250ms/500ms stimuli, shutdown cancellation and error/capture limits. No live sockets/hardware.60s operator rule adds no callback or timer. See experiment ledger; execution remains unauthorized.

## Approved G4-A correction - callback bodies unchanged

All four Node callbacks (onRequest, onListening, onServerError, onResponseError), their bodies, signatures and registration tail match accepted f7d0f1682d835611f61b658ff2e60b4607ac704d exactly. Only two SERVER_OPTIONS values change; no helper, task or function added. Mocked createServer observes reuseAddr=false/sendAcksForNonConfirmablePackets=true. Existing 14 request-validation cases and 11 option/endpoint/error checks pass (25/25); mocked errors do not prove actual Windows bind rejection or network transmission. Five ESP32 and eleven fixture functions remain untouched.

## Approved P2-07 fixture Content-Format correction

Base c3523ef9ac17106a726f43c04998e163fbc1f1bb. Only onFrontMessage's existing probe-response Content-Format validation changes. Present option must have 0..2 bytes and all bytes zero (numeric text/plain identifier0); empty,00,0000 accepted; absent/nonzero/oversized rejected. Existing error cleanup is reused. No helper/callback/signature added: eleven fixture functions/sixteen modes unchanged, four server and five ESP32 functions untouched. Code/token/MID/payload, prohibited options, source-peer checks and separate-CON ACK completion path unchanged.54/54 corrected mocked checks pass; no real P2-07 rerun yet.
