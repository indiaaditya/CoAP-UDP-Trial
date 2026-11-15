# Phase 2 test execution boundary — G3-A

G4 is not authorized. Do not flash, monitor hardware, run network campaigns, alter AP/firewall settings or claim acceptance from this document. G3-A was accepted; G3-B additionally validates instrumentation and separate clean builds/configuration/size attribution. Neither stage authorizes runtime execution.

The approved G2 matrix remains the future test scope: P2-01 identities; 02 missing private header; 03 primary build; 04 scoped build; 05 configuration/secret review; 06 server syntax/start/conflict; 07 valid server probe; 08 negative probes; 09 deployment/boot; 10 healthy 20/20; 11 independent reset; 12 packet capture; 13 dropped request; 14 dropped response; 15 duplicate request; 16 duplicate response; 17 separate response; 18 duplicate separate CON; 19 blackhole retry exhaustion; 20 empty ACK only; 21 wrong token; 22 wrong code/body; 23 malformed packet; 24 wrong port; 25 RST; 26 primary resources; 27 scoped resources; 28 lifecycle/tasks; 29 Beken portability.

Only P2-01/P2-03/P2-05 and the syntax portion of P2-06 have G3-A evidence where documented. All runtime and fault checks remain Not Executed. G3-C fixture is implemented and validated with mock sockets/timers only. Detailed execution setup, expected/actual logs, PASS/FAIL and captures must be recorded at the appropriate authorized stage. Final separate-CON ACK must be observed before cleanup; synchronous library ACK invocation alone does not prove delivery.

G3-B provides P2-04 scoped clean-build evidence and expanded P2-03/P2-05 build/configuration evidence only. P2-26/P2-27 runtime readings remain Not Executed. Use primary for historical whole-run comparisons; use scoped only for B4 plus disclosed monitor overhead. Both require independently identified G4 runs.


## G3-C fixture usage (for future authorized G4 only)

Do not execute these network commands until G4 is authorized. Current validation uses mocked sockets/timers only.

From phase2-coap-baseline, after dependencies are installed under src:

```text
node test/coap_fixture.js MODE [CASE]
```

Proxy modes(pass/drop-first-request/drop-first-response/duplicate-request/duplicate-response): run unchanged cloud.js separately with TRIAL_COAP_PORT=56831. Fixture fronts56830 and forwards through one stable UDP socket to127.0.0.1:56831. Wait for front/back listening lines before device run; stop test-owned processes afterward. These are fault proxies, not product proxy functionality.

Synthetic modes: stop the normal server on56830; start selected fixture instead. Wait for front listening line (and wrong-port socket line for wrong-port). Use separate/separate-duplicate/blackhole/ack-only/wrong-token/wrong-code/malformed/wrong-port/rst, or bad-payload CASE FAIL(default), LF or OVERSIZE. Separate response delay250ms, duplicate500ms from receipt; nominal host timers require G4 capture verification. Fixture maintains current response identity across repeated request, no general server retry engine. Modes can serve20 sequential trial requests; watchdog120000ms/capture256 bound the run.

Probe mode: run standard cloud.js on56830; fixture binds ephemeral loopback port and sends one request. CASE valid(default), path, method, payload or format. Validate expected code/body/token/ACK MID/content format. Empty ACK alone is incomplete; separate CON probe response is ACKed before successful probe cleanup. Probe does not implement application retries or a device emulator.

SIGINT/SIGTERM: fixture clears timers and closes owned sockets once. STOPPED/exit0 indicates operator shutdown only, not successful device test. Watchdog/capture/socket/parse failures exit1. Fault-mode outcomes must be assessed from device logs/captures, not fixture exit status. Wrong-peer datagrams are ignored; use one device per run. Datagram counts are not logical transaction counts. Metadata record includes direction/time/length/MID/token/code/action; no payload dump.

Request payload length correction: specified CoAP request encodes to24 bytes, not prior arithmetic23. Header4+token4+URI-Path10+Content-Format1+marker1+PING4. No protocol design change.
