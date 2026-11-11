# Experiment comparison

Evidence updated: 2026-11-11. Phase 1 Steps 1 and 2 have been exercised; no cross-phase comparison is currently available.

| Experiment | Environment | Actual evidence | Limits |
| --- | --- | --- | --- |
| Phase 1, Step 1 | Windows x64, Node.js v25.9.0, localhost | 20 sequential PING/PONG UDP request/response exchanges plus one liveness exchange; 21/21 responses; five unknown payloads received no reply within 500 ms each; occupied-port error observed | T07 subsequently passed by user-reported manual verification; no embedded or remote-network results |

Detailed evidence is retained in phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md. No performance, resource, security or protocol-superiority comparison is established. Future results must retain historical evidence.


## Phase 1, Step 2 evidence update (2026-11-11)

ESP32-S3 revision v0.2, 16 MB QIO flash, physical 8 MB PSRAM disabled, ESP-IDF 5.5.4 C/FreeRTOS. After explicitly approved target recovery and private network configuration correction, two independent positive hardware runs completed 20/20 UDP request/response exchanges each, 40/40 combined. Five controlled negative runs each stopped after one attempt with no accepted response: absent server, FAIL, PONG+LF, oversized response and wrong source port. Preserve the initial boot failure, unavailable-network observation and one association setup failure separately; these are not silently retried or aggregated into a packet-loss metric.

Main source unchanged from accepted G3. Final application binary 757920 bytes. Observed minimum internal heap in the two positive runs: 275460 and 274008 bytes; main stack minimum free: 1748 bytes in both. These are observed values, not guaranteed worst-case bounds. Suite status: 12 PASS, 2 AP tests unexecuted by user direction. Detailed evidence is in phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md. No architectural superiority or phase acceptance conclusion is drawn.


## Step 3 consolidation (2026-11-11)

The completion report, resource table, exact source/image identities and full effective non-secret build configuration are consolidated in phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md. No new experiment or cross-phase result was added during closure. Step 1 observations (21/21 positive localhost responses) and Step 2 observations (40/40 positive hardware responses across two runs) remain distinct; intentional negative tests and association setup failures are not combined into a packet-loss rate. No comparison supports choosing UDP over CoAP or another long-term architecture. Milestone acceptance remains pending independent review and user decision.
