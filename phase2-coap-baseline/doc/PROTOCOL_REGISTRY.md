# Phase 2 protocol registry — G3-A

Unsecured CoAP v1 over unicast IPv4 UDP. Peer 192.168.0.170:56830. Node binds 0.0.0.0, default 56830; optional TRIAL_COAP_PORT is validated as 1..65535 for later isolated backend tests.

| Element | Implemented value |
| --- | --- |
| Request | CON POST (0.02), URI-Path coap-test, Content-Format 0 (zero-length integer), exact four-byte PING |
| Success response | 2.04 Changed, Content-Format 0, exact four-byte PONG |
| Token | Four actual bytes; eight-byte library API output buffer; randomized normalized four-byte initial seed; subsequent library tokens |
| MID | Library-created for each logical request; library retains for retransmissions |
| Responses | ACK with matching request MID, or separate CON/NON correlated by token |
| Reliability | ACK timeout 2 seconds, factor 1.5, MAX_RETRANSMIT 4, NSTART 1 |
| Application ceiling | 100000 ms from initial send; unsigned FreeRTOS elapsed ticks; ACK does not restart deadline |
| I/O service | <=100 ms requested waits, including 1000 ms pacing after each success except final |
| Campaign | At most 20 logical attempts; increment before request construction/submission; success counted once in main; stop on terminal failure |

Expected UDP payload lengths (calculations, not captures): 23-byte request, 14-byte piggyback response, four-byte empty ACK. Additional options would alter these values.

Reject matching wrong code/body/format, Observe and Block1/Block2; reject wrong ACK MID. Wrong token never succeeds. FAIL intentionally results in library RST for CON/NON where supported; ACK responses are not reset. Library parser can reject malformed packets before callback. Libcoap may consume an invalid-token ACK at message layer: no guaranteed recovery from every malformed peer response; no false application success and bounded termination.

Node uses package duplicate cache, piggyback handling and retransmissions, with 65536-byte cache cap, 50 ms piggyback threshold, IPv4, proxy false, ACK-for-NON false. It does not implement faults/delays. Internal excluded features may still exist in the package; no Observe/discovery/blockwise/proxy/multicast/security transactions are intentionally initiated.

Excluded: MQTT, future presence/pending-message/MQTT handover, production heartbeat/adaptive polling, cloud push, certificates, DTLS, OSCORE, registration, ECDH/HKDF/AES-GCM integration, large transfers, Redis, production recovery and generalized transports. No runtime protocol evidence yet.
