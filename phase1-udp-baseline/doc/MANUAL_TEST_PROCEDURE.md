# Phase 1 — Manual UDP Trial Procedure

**Project:** MQTT Alternative Route / CoAP-UDP-Trial  
**Phase:** 1 — Raw UDP Baseline  
**Document:** MANUAL_TEST_PROCEDURE.md  
**Revision:** 1.0  
**Development Branch:** `phase1-udp-baseline`  
**Repository:** https://github.com/indiaaditya/CoAP-UDP-Trial

---

## 1. Purpose

This document describes the procedure for manually testing UDP communication between an ESP32-S3 and a NodeJS server.

The objective is to verify:

1. ESP32 Wi-Fi connectivity.
2. ESP32-to-NodeJS UDP communication.
3. NodeJS-to-ESP32 UDP responses.
4. Successful repeated request/response exchanges.
5. Basic timeout and failure handling.
6. ESP32 runtime resource observations.
7. Repeatability of the experiment after resetting the ESP32.

The procedure allows an engineer to repeat Phase 1 testing without modifying the source code or requiring Codex assistance.

The experiment uses raw UDP only. CoAP, MQTT, security protocols, and production heartbeat functionality are outside its scope.

---

## 2. Test Architecture

```text
                    Local Wi-Fi Network
                  (2.4 GHz, same subnet)

       ESP32-S3                         Windows PC
       UDP Client                       NodeJS Server
           |                                  |
           |                                  |
           |---------- UDP PING ------------->|
           |                                  |
           |<--------- UDP PONG --------------|
           |                                  |
           |     Repeat up to 20 times        |
           |                                  |
           |                                  |
      Log results                       Log packets
      and resources                     and replies
```

### Communication parameters

| Parameter | Value |
|---|---|
| Transport | UDP over IPv4 |
| ESP32 operating mode | Wi-Fi Station |
| Server IPv4 address | `192.168.0.170` |
| Server UDP port | `56830` |
| Request payload | ASCII `PING` |
| Response payload | ASCII `PONG` |
| Request payload size | 4 bytes |
| Response payload size | 4 bytes |
| Maximum exchange attempts | 20 |
| Interval between successful exchanges | 1 second |
| UDP receive timeout | 2 seconds |
| Wi-Fi startup/connection deadline | 30 seconds |

The ESP32 receives its IPv4 address through the local Wi-Fi network.

The ESP32 source UDP port is assigned by the networking stack and may change after a reset.

The NodeJS server always replies to the IP address and port from which the request was received.

A successful UDP exchange requires the ESP32 to receive and validate exact `PONG`.

---

## 3. Hardware and Software Requirements

### 3.1 Hardware

| Item | Requirement |
|---|---|
| Microcontroller module | ESP32-S3-WROOM-1 |
| Flash | 16 MB |
| PSRAM | 8 MB physically available; disabled in Phase 1 firmware |
| Development interface | USB/UART serial |
| Serial port used during original tests | COM14 |
| Host computer | Windows PC |
| Network | Wi-Fi supporting 2.4 GHz |

The COM port may change if the board is connected to another USB port or computer.

### 3.2 Software

| Software | Phase 1 baseline |
|---|---|
| ESP-IDF | v5.5.4 |
| NodeJS | v25.9.0 |
| Operating system | Windows |
| Command terminal | PowerShell |
| Embedded language | C |
| UDP API | ESP-IDF bundled lwIP BSD sockets |
| NodeJS UDP API | Built-in `node:dgram` |

### 3.3 Repository files

**NodeJS server**

`phase1-udp-baseline/src/cloud.js`

**ESP32 firmware**

`phase1-udp-baseline/src/esp32-udp-client/main/main.c`

**ESP32 project directory**

`phase1-udp-baseline/src/esp32-udp-client/`

**Existing experiment results**

`phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md`

**Resource measurements**

`phase1-udp-baseline/doc/RESOURCE_BASELINE.md`, when available.

---

## 4. Preconditions

Before beginning the manual trial, verify the following:

- ESP32 contains the approved Phase 1 UDP firmware.
- ESP32 is connected to the Windows PC.
- ESP32 is powered correctly.
- Windows PC is connected to the intended Wi-Fi network.
- ESP32 Wi-Fi credentials correspond to the same network.
- NodeJS is installed.
- The configured server address is reachable from the ESP32 network.
- Windows Firewall permits the required UDP communication.
- No other process occupies UDP port 56830.
- A serial monitoring utility is available.

The originally tested server address is `192.168.0.170`.

If the Windows PC receives a different IPv4 address, the configured ESP32 destination must correspond to the actual server address. Changing the firmware configuration requires a separately approved rebuild and flash; it is not part of this manual procedure.

**Important:** This procedure assumes that the approved firmware is already installed.

Do not erase flash, modify eFuses, change flash encryption settings, or reflash the board merely to conduct the manual trials.

---

## 5. Trial MT-01 — Normal UDP Communication

### Objective

Verify successful bidirectional communication between the ESP32-S3 and the NodeJS server.

### Step 1 — Start the NodeJS server

Open PowerShell.

Navigate to the repository root.

Execute:

```powershell
node phase1-udp-baseline/src/cloud.js
```

Expected output:

```text
Listening on UDP 0.0.0.0:56830
```

This indicates that the server has bound to UDP port 56830.

Leave the terminal running.

The address `0.0.0.0` indicates that the server listens on all available local IPv4 interfaces.

### Step 2 — Open the ESP32 serial monitor

Open another PowerShell terminal.

Initialize the ESP-IDF v5.5.4 environment using the installed ESP-IDF terminal or its export script.

For the original Windows setup, the export script is:

```powershell
. C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1
```

The ESP-IDF Python environment must be correctly activated. If necessary, use the ESP-IDF PowerShell shortcut supplied with the installation.

Navigate to:

```text
phase1-udp-baseline/src/esp32-udp-client/
```

Execute:

```powershell
idf.py -p COM14 monitor
```

The console uses 115200 baud.

If COM14 is unavailable, identify the actual ESP32 serial port and substitute it.

Only one serial-monitoring program should have the port open.

### Step 3 — Reset the ESP32

Press the ESP32 board RESET/EN button once.

Observe the ESP32 serial monitor.

The expected startup sequence is:

1. ESP32 boots.
2. ESP-IDF initializes.
3. Wi-Fi initialization completes.
4. ESP32 connects to the configured access point.
5. An IPv4 address is assigned.
6. UDP socket setup completes.
7. The firmware begins sending `PING`.

### Step 4 — Observe the NodeJS console

For every valid request, NodeJS should report messages similar to:

```text
Received from 192.168.0.122:56788 bytes=4 payload="PING"
Sent PONG to 192.168.0.122:56788
```

The IPv4 address and UDP source port shown here are examples from previous trials.

The source port should remain consistent throughout one firmware run, because the same UDP socket is reused.

The port may change after resetting the ESP32.

### Step 5 — Observe the ESP32 console

The ESP32 serial monitor should show successful UDP exchanges.

Representative messages:

```text
Exchange 1 sent PING: bytes=4
Exchange 1 received exact PONG: bytes=4 successes=1
```

The firmware should continue through 20 successful exchanges.

Expected final result:

```text
Result: attempts=20 successes=20 status=PASS
```

This message confirms that the ESP32 itself received and validated all 20 responses.

The NodeJS console alone cannot establish successful ESP32 reception.

### Step 6 — Observe completion

After 20 successful exchanges:

- The ESP32 stops sending UDP messages.
- The firmware logs its resource observations.
- The firmware closes its UDP socket.
- Wi-Fi is stopped.
- Application-owned resources are cleaned up as designed.
- `app_main()` returns.
- The NodeJS server continues listening.

A representative completion indication is:

```text
main_task: Returned from app_main()
```

A Wi-Fi disconnect event caused by the intentional shutdown may appear after the PASS result. This should not automatically be treated as a communication failure.

### Pass criteria

MT-01 passes when:

- NodeJS receives 20 exact `PING` requests.
- NodeJS sends corresponding `PONG` responses.
- ESP32 reports 20 attempted exchanges.
- ESP32 reports 20 successful exchanges.
- Final firmware status is PASS.
- The application completes without an unexpected reset or panic.

---

## 6. Trial MT-02 — Repeatability After Reset

### Objective

Verify that resetting the ESP32 starts a new independent UDP experiment.

### Procedure

1. Keep the NodeJS server running.
2. Allow the previous firmware run to finish.
3. Press RESET/EN on the ESP32.
4. Observe the serial monitor and NodeJS output.
5. Confirm the exchange counters restart from the initial state.
6. Observe a complete new 20-exchange run.

### Expected result

```text
Result: attempts=20 successes=20 status=PASS
```

The firmware should produce another independent run.

A different UDP source port is acceptable and expected to be possible.

The source port is not a persistent device identity.

### Pass criteria

- Firmware restarts successfully.
- Communication resumes.
- Exchange count begins anew.
- All 20 exchanges succeed.

---

## 7. Trial MT-03 — NodeJS Server Unavailable

### Objective

Verify that the ESP32 detects the absence of the UDP server and does not retry indefinitely.

### Procedure

1. Stop the NodeJS server using Ctrl+C.
2. Keep the Wi-Fi network operational.
3. Keep the ESP32 serial monitor open.
4. Reset the ESP32.
5. Observe startup, Wi-Fi association and UDP behavior.

### Expected behavior

ESP32 should:

1. Initialize successfully.
2. Connect to Wi-Fi.
3. Acquire an IPv4 address.
4. Attempt to send `PING`.
5. Wait for a response.
6. Detect the missing response or a socket-level error.
7. Stop without retransmitting the request.
8. Report the failure and enter cleanup.

Representative output:

```text
Exchange 1 receive timeout: errno=11
Result: attempts=1 successes=0 status=FAIL
```

The exact error code and timing may depend on network behavior.

If Wi-Fi association fails before UDP transmission, that is a setup failure, not a valid server-unavailable test. Record the observation separately.

### Pass criteria

- Wi-Fi connects successfully.
- One UDP attempt is made.
- No valid response is accepted.
- No automatic UDP retransmission occurs.
- Firmware reports failure and stops the experiment.

After the test, restart the NodeJS server before conducting positive trials.

---

## 8. Trial MT-04 — UDP Source Port Observation

### Objective

Observe the UDP source port assigned to the ESP32 across independent runs.

### Procedure

1. Start the NodeJS server.
2. Reset the ESP32.
3. Record the source UDP port displayed by NodeJS.
4. Allow the trial to complete.
5. Reset the ESP32 again.
6. Record the source UDP port from the new run.

### Expected behavior

Within one run, the same socket should normally retain the same source port.

Between resets, the source UDP port may change.

Example:

| Run | ESP32 IPv4 | Source UDP Port |
|---|---|---|
| Run 1 | 192.168.0.122 | 60219 |
| Run 2 | 192.168.0.122 | 56788 |

These are examples from previously observed tests, not fixed values.

### Pass criteria

- NodeJS replies to the actual source port of each request.
- ESP32 communication succeeds regardless of the assigned ephemeral port.

A port change between resets is not mandatory for passing the test.

---

## 9. Trial MT-05 — ESP32 Resource Observation

### Objective

Record the runtime memory observations for future comparison with CoAP.

### Procedure

1. Start the NodeJS server.
2. Open the ESP32 serial monitor.
3. Reset the ESP32.
4. Allow all 20 exchanges to complete.
5. Record the memory measurements shown in the ESP32 console.

### Required measurements

| Measurement | Unit |
|---|---|
| Free internal heap before Wi-Fi | Bytes |
| Free internal heap after IPv4 acquisition | Bytes |
| Free internal heap at result, before cleanup | Bytes |
| Minimum observed free internal heap | Bytes |
| Main task minimum free stack | Bytes |
| Firmware result | PASS / FAIL |

The original Phase 1 implementation already produces these measurements.

No additional instrumentation is required.

### Existing reference observations

| Measurement | Recorded Phase 1 value |
|---|---:|
| Internal heap free before Wi-Fi | 329724 bytes |
| Internal heap free after IPv4 | 278212 bytes |
| Internal heap free before cleanup | 277576 bytes |
| Lowest observed free internal heap | 274008 bytes |
| Main task minimum free stack | 1748 bytes |

These values are references from earlier successful runs.

They are not mandatory values for subsequent runs. Runtime measurements may vary.

The minimum free heap and stack high-water mark are observed values, not guaranteed worst-case resource requirements.

Do not interpret free internal heap as total physical RAM or the full networking stack's memory usage.

### Pass criteria

- Firmware completes the trial.
- Resource values are recorded.
- No unexpected memory-related failure, assertion or panic is observed.

A change in free heap alone does not constitute a failed trial without an established acceptance threshold.

---

## 10. Manual Trial Summary

| Test ID | Description | Expected result |
|---|---|---|
| MT-01 | Normal UDP exchange | 20/20 successful |
| MT-02 | Repeatability after reset | Independent 20/20 run |
| MT-03 | UDP server unavailable | Single failed attempt; no retry |
| MT-04 | Source UDP port observation | Correct endpoint handling |
| MT-05 | Resource measurement | Valid observations recorded |

This manual suite complements the controlled automated tests already performed during Phase 1.

The original hardware test suite also included malformed-response and wrong-source-port checks.

Those tests are not repeated here because they require special controlled UDP responders.

The AP-unavailable and mid-run AP-disconnect tests remain outside this manual procedure.

---

## 11. Manual Test Observation Record

Complete the following for each test run.

### Test identification

| Field | Recorded value |
|---|---|
| Test ID | |
| Test date | |
| Engineer | |
| Git branch / commit | |
| Firmware identity / hash, if known | |
| ESP-IDF version | |
| NodeJS version | |
| ESP32 serial port | |
| Windows PC IPv4 | |
| ESP32 IPv4 | |
| ESP32 source UDP port | |

### Communication observations

| Parameter | Actual observation |
|---|---|
| UDP requests observed by NodeJS | |
| PONG responses sent by NodeJS | |
| UDP attempts reported by ESP32 | |
| Successful responses reported by ESP32 | |
| Timeouts | |
| Unexpected responses | |
| Firmware final status | |
| Clean completion observed | |

### Resource observations

| Parameter | Actual value (bytes) |
|---|---|
| Free heap before Wi-Fi | |
| Free heap after IPv4 | |
| Free heap before cleanup | |
| Minimum free internal heap | |
| Minimum free main task stack | |

### Final assessment

| Field | Recorded value |
|---|---|
| Result | PASS / FAIL / NOT EXECUTED |
| Failure reason, if any | |
| Observations | |
| Relevant console logs retained | Yes / No |

Do not overwrite previously recorded results when performing a new trial.

Each trial must remain independently identifiable.

---

## 12. Interpreting the Results

### 12.1 NodeJS receives PING but ESP32 reports failure

Possible causes include:

- Response timeout.
- Unexpected response.
- Wi-Fi disconnection.
- Socket error.

Review the ESP32 serial output before drawing conclusions.

NodeJS logging a successful `socket.send()` completion does not prove that the ESP32 received the datagram.

### 12.2 ESP32 sends fewer than 20 requests

The firmware intentionally stops on the first detected failure.

Inspect the final attempt count and failure logs.

Do not assume that an incomplete run indicates a source-code defect.

### 12.3 Source UDP port changes after reset

This is normal.

UDP does not establish a persistent network connection merely because the firmware calls `connect()` on its UDP socket.

### 12.4 Source IP reported as 0.0.0.0

The ESP32 `getsockname()` observation may display `0.0.0.0` for its locally wildcard-bound UDP socket.

Use the acquired Wi-Fi IPv4 address and NodeJS reception logs to identify the actual source interface address.

### 12.5 Unexpected reboot or panic

Record the serial logs and classify the test as failed or incomplete.

Do not erase flash, modify security settings, or attempt automatic firmware recovery as part of this procedure.

Report the issue for review.

---

## 13. Test Completion and Equipment Shutdown

After testing:

1. Confirm the current firmware run has completed.
2. Record the final test results.
3. Stop NodeJS using Ctrl+C.
4. Stop the ESP32 serial monitor, normally using Ctrl+].
5. Close the PowerShell terminals when no longer needed.
6. Leave the board in its existing approved configuration.

Do not perform a whole-chip erase or change ESP32 eFuses.

Do not alter flash encryption or secure-boot settings.

No source-code modifications or repository commits are needed merely to perform these manual trials.

---

## 14. Limitations

The manual trials demonstrate communication under the tested local-network conditions.

They do not establish:

- Guaranteed UDP delivery.
- Internet-wide reliability.
- Long-term Wi-Fi stability.
- NAT traversal behavior.
- Packet delivery under congestion.
- Concurrent multi-device performance.
- Production heartbeat behavior.
- End-to-end cryptographic security.
- Final suitability of UDP versus CoAP or MQTT.

Only the actual experimental observations should be recorded.

Architectural interpretation belongs to the project architecture review process.

---

## 15. Future Use

This procedure should remain available as a reproducible Phase 1 reference.

When Phase 2 introduces CoAP, the corresponding manual test procedure should retain equivalent:

- Hardware and SDK configuration.
- Test-network conditions.
- Number of exchanges.
- Payload/workload assumptions, with protocol differences documented.
- Resource measurement checkpoints.
- Result-recording format.

Any additional CoAP overhead should be measured and compared against the raw UDP baseline without changing or overwriting historical Phase 1 observations.

**Document status:** Manual verification procedure for the approved Phase 1 UDP baseline. Execution results must be recorded separately in `EXPERIMENT_RESULTS.md` or an approved test record.