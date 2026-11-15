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

Expected UDP payload lengths (calculations, not captures): 24-byte request, 14-byte piggyback response, four-byte empty ACK. Additional options would alter these values.

Reject matching wrong code/body/format, Observe and Block1/Block2; reject wrong ACK MID. Wrong token never succeeds. FAIL intentionally results in library RST for CON/NON where supported; ACK responses are not reset. Library parser can reject malformed packets before callback. Libcoap may consume an invalid-token ACK at message layer: no guaranteed recovery from every malformed peer response; no false application success and bounded termination.

Node uses package duplicate cache, piggyback handling and retransmissions, with 65536-byte cache cap, 50 ms piggyback threshold, IPv4, proxy false, ACK-for-NON false. It does not implement faults/delays. Internal excluded features may still exist in the package; no Observe/discovery/blockwise/proxy/multicast/security transactions are intentionally initiated.

Excluded: MQTT, future presence/pending-message/MQTT handover, production heartbeat/adaptive polling, cloud push, certificates, DTLS, OSCORE, registration, ECDH/HKDF/AES-GCM integration, large transfers, Redis, production recovery and generalized transports. No runtime protocol evidence yet.


## G3-C stimulus modes and calculation correction

Approved wire content unchanged. Correct request size is **24 bytes**, not the earlier arithmetic prediction23: header4 + token4 + URI-Path option10 (one-byte header plus nine-byte coap-test) + Content-Format option1 + marker1 + PING4. The actual codec output verifies24; piggyback PONG14 and empty ACK4 remain correct. This is a calculation correction, not a protocol/firmware design change or captured-wire result.

| Mode | Behavior |
| --- | --- |
| pass | Front56830 -> stable backend socket -> server127.0.0.1:56831; unchanged bytes in both directions |
| drop-first-request | Suppress first front datagram; subsequent bytes forwarded unchanged |
| drop-first-response | Suppress first backend datagram; subsequent responses forwarded |
| duplicate-request | Forward first front datagram immediately and identical copy after250ms |
| duplicate-response | Forward first backend response immediately and identical copy after250ms |
| blackhole | Log valid request stimuli; no response |
| separate | Immediate empty matching-MID ACK; after250ms CON2.04/PONG with original token and new MID(request+1 modulo65536) |
| separate-duplicate | Separate behavior plus identical CON copy at500ms; incoming ACK/RST logged, never counted as new request success |
| ack-only | Empty matching-MID ACK, no application response |
| wrong-token | Piggyback2.04/PONG with first token byte toggled; length and MID preserved |
| wrong-code | Matching-token/MID4.00/PONG |
| bad-payload | Matching2.04 with CASE FAIL(four bytes), LF(PONG newline, five), or OVERSIZE(100X bytes) |
| malformed | Three-byte truncated header, then correct piggyback response after250ms |
| wrong-port | Correct PONG from a separately bound ephemeral socket |
| rst | Empty matching-MID RST |
| probe | Single CON to127.0.0.1:56830; CASE valid/path/method/payload/format expects2.04/4.04/4.05/4.00/4.00 |

Synthetic replies use explicit text/plain0 and unchanged token except wrong-token. Repeated identical requests in separate modes get ACK but no fresh scheduled application response/MID. Twenty sequential distinct transactions are supported within capture cap. These simple stimuli do not add general separate-response retransmission to the fixture: real-server retry/cache behavior belongs to proxy-based G4 tests.

Unexpected front/backend peers are ignored. Probe wrong token/code/body/format/ACK MID fails; empty ACK alone never completes. A separate probe CON is acknowledged and success waits for its four-byte ACK send callback (not proof of delivery). Proxy/blackhole operator shutdown is STOPPED, not test PASS. Negative-device outcomes are assessed from device/capture evidence at G4, not fixture exit0. Watchdog/capture/socket/parse errors fail and clean up.


## Approved host watchdog amendment - no protocol change

Host fixture safety ceiling180000ms replaces120000ms. Firmware Wi-Fi budget30000ms, application ceiling100000ms, CoAP ACK/retransmission parameters and250ms/500ms fault stimulus timing are unchanged. First device request must be observed within60s of fixture launch; enforce manually as a setup-validity condition, not new source logic. At first request60s,100s application waiting ends around160s with20s host margin;93s waiting ends around153s with27s margin. Late setup/fixture timeout is fixture INVALID/overall BLOCKED; retain ESP result independently. Captured-wire evidence and runtime measurement validity remain separate. No network test performed by this amendment.
