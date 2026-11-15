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
trial_error_t: TRIAL_ERROR_NONE, BUILD, SEND, NACK, IO, DEADLINE, RESPONSE, WIFI, MEASUREMENT with full TRIAL_ERROR_ prefixes. MEASUREMENT records scoped-monitor errors under G3-B.

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

send_ping_request local pdu is coap_pdu_t *, NULL initially; owned until coap_send, which consumes even on failure. Callback token is coap_bin_const_t; received PDU/data/options are borrowed. G3-B stores reached measurement values before logging; the added state is listed below.

## Node module

coap: module object; COAP_PORT: number, default 56830 or validated TRIAL_COAP_PORT; BIND_ADDRESS: string 0.0.0.0; RESOURCE_PATH: string /coap-test; PING_PAYLOAD/PONG_PAYLOAD: Buffer, four bytes; SERVER_OPTIONS: object (udp4, reuseAddr false, proxy false, piggybackReplyMs 50, sendAcksForNonConfirmablePackets true, cacheSize 65536); server: package server; server_failed: boolean initially false. Process lifetime. Callback req/res/error are borrowed invocation arguments; no custom persistent device state.


## G3-B measurement state (no new function or task)

All variables are automatic in app_main, initialized to zero/false; no extra persistent protocol state.

| Names | Exact type | Scope / purpose |
| --- | --- | --- |
| measurement_failed | bool | Both builds; false, set on scoped monitor error; separate evidence status |
| heap_b1, heap_b2, heap_b3, heap_b5 | size_t each | Both builds; reached checkpoint values; never publish initialization zero as an unreached reading |
| main_stack_minimum | UBaseType_t | Both builds; pre-cleanup result-stage high-water mark, bytes |
| task_count_before_coap, task_count_after_coap | UBaseType_t each | Both builds; snapshots before startup and after session/token initialization; observation, not proof of library task ownership |
| whole_run_minimum | size_t | Primary only; unreset SDK watermark |
| scoped_monitor_active | bool | Scoped only; becomes true after start; stop before CoAP cleanup |
| heap_b4 | size_t | Scoped only; read while monitor active, before stop |
| heap_before_monitor, heap_after_monitor | size_t each | Scoped only; same pool before/after monitor start; SDK activity included |

CONFIG_TRIAL_SCOPED_HEAP_MONITOR is a Kconfig bool, default n. sdkconfig.scoped.defaults overrides y. No allocated project measurement buffer. SDK monitor itself owns allocated bookkeeping until stop; its allocation failure can assert. No application-owned task or additional callback.


## G3-C fixture state - test/coap_fixture.js

All module state is owned by one fixture process and released at finish. No firmware or standard-server state changed.

| Name | Type / initial value | Purpose / ownership |
| --- | --- | --- |
| dgram | Node built-in module object | UDP socket factory; mocked for G3-C validation |
| coapPacket | Public coap-packet module object, version1.1.1 | parse/generate only |
| FRONT_PORT | const number56830 | Device-facing port; probe target |
| BACKEND_ADDRESS, BACKEND_PORT | const string127.0.0.1, const number56831 | Isolated real-server backend in proxy modes |
| SEPARATE_DELAY_MS, DUPLICATE_DELAY_MS | const number250 each | Delayed separate/duplicate stimuli |
| FIXTURE_TIMEOUT_MS | const number180000 | Approved amended host run watchdog; firmware transaction timeout remains100000 |
| MAX_CAPTURED_DATAGRAMS | const number256 | Metadata-record limit; fail closed on limit |
| mode, payload_case | string or undefined, argv[2]/argv[3] | Validated mode and optional CASE |
| front, back, wrong_port_socket | dgram.Socket or null, initially null | Owned sockets, closed once; back only proxy modes; wrong_port_socket only wrong-port |
| client | {address:string, port:number} or null | Locked first parsed front peer for non-probe modes |
| request_count, response_count | number0 | Front/backend ingress datagrams; not logical transactions/success counts |
| first_request, first_response | Buffer or null | Proxy: first captured datagram; synthetic: first request/response of the current stimulus transaction, reused for repeated identical request |
| scheduled_timers | const Set of timer handles, empty | Bounded scheduled sends; cleared on finish; fired handles retained only until bounded run ends |
| watchdog | timer handle or null | Single run watchdog |
| ended | boolean false | Idempotent terminal guard |
| capture | const Array, empty | Bounded metadata only: direction, time, length, MID, token, code, action; no payload dump |
| probe_request | packet object or null | Single deterministic CON request, MID42, token8000002a, path/method/body/format variation |
| probe_expected | expectation object or null | Expected code/payload; accepted flag permits completion only after separate-CON ACK send callback |
| probe_sent | boolean false | Prevent second probe submission |

Routine packet/reply objects, temporary ACK bytes, repeated-request predicates, option iteration and parsed metadata are invocation-local. Scheduled sends hold copied buffers until bounded timers/finish. Only codec generation/parsing and test fault actions occur; no adaptive policy/security/application task.


## Approved watchdog-only amendment (review candidate)

Only FIXTURE_TIMEOUT_MS changes120000 ->180000. No function/mode/dependency/socket/stimulus timer/state addition. The60-second first-request readiness rule is an operator validity rule in MANUAL_TEST_PROCEDURE.md, not a new variable or fixture behavior. First request late/missing ->setup INVALID/overall BLOCKED, not ESP protocol failure.

## Approved G4-A server-option correction

Based on accepted f7d0f1682d835611f61b658ff2e60b4607ac704d. Only SERVER_OPTIONS changes: explicit reuseAddr=false and sendAcksForNonConfirmablePackets=false -> true. The object remains file-scope, process-lifetime input to coap.createServer; no new variable, callback or persistent state. The latter flag is required by the inspected coap 1.6.0 outgoing-packet send gate to permit piggyback ACK responses; its name must not be interpreted as proof that the server now constructs ACKs for NON requests. Broader package send gating is affected; only the approved CON workload is in scope. Real bind/delivery verification remains pending.

## P2-07 probe Content-Format validation correction

Existing local boolean content_format in onFrontMessage remains false until a Content-Format option representing numeric0 is found. RFC7252 permits leading-zero uint decoding; option12 is restricted to0..2 bytes. Each encountered value is compared to a same-length zero Buffer after the >2 length rejection. Buffer.alloc temporary is at most2 bytes due short-circuit evaluation; no persistent variable, new timer/state/constant or numeric conversion overflow. Missing option still fails. No packet generation/delay/socket/watchdog variable changes. Semantic format0 is distinct from raw encoding empty/00/0000.
