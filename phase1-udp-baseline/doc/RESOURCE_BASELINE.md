# Phase 1 ESP32 Resource Baseline — Raw UDP

Documentation date: 2026-11-11 (Asia/Calcutta). Measurements were obtained during the recorded Phase 1 build/G4 work, not during creation of this document. No firmware/configuration changes, rebuild, device access or additional measurement was performed for this task. This document does not declare milestone acceptance.

## Evidence identifiers and run definitions

Paths below are relative to this document unless otherwise stated. Existing local logs are ignored artifacts; the significant raw values used from them are transcribed here so this document remains reviewable from Git.

| ID | Existing source | Meaning |
| --- | --- | --- |
| B1 | [EXPERIMENT_RESULTS.md — G3 implementation and build evidence](EXPERIMENT_RESULTS.md#phase-1-step-2--g3-implementation-and-build-evidence), especially Build artifact observations | Final-source G3 build before private network correction; section sizes are build measurements from this image, not a separate size report for the final G4 image |
| B2 | [EXPERIMENT_RESULTS.md — Step 3 reproducibility archive](EXPERIMENT_RESULTS.md#step-3-reproducibility-archive--existing-artifacts-only-2026-11-11) and its complete effective sdkconfig snapshot | Source/artifact hashes, complete non-secret configuration and pinned SDK submodules |
| H1 | [EXPERIMENT_RESULTS.md — G4 recovery, retest and final available results](EXPERIMENT_RESULTS.md#g4-recovery-retest-and-final-available-results-2026-11-11), Run observations and Runtime resource observations | Final tested image, two positive runs, heap/stack readings, network and recovery conditions |
| R1 | H1 Corrected-network baseline; existing local build/g4-corrected-network-baseline-sanitized.log | First successful final-image hardware run: 20 attempts, 20 successes; cloud source 192.168.0.122:59185 |
| R2 | H1 Reset/completion run; existing local build/g4-reset-completion-sanitized.log | Second successful final-image hardware run after external UART RTS reset: 20 attempts, 20 successes; cloud source 192.168.0.122:60946 |
| L1 | Existing local build/log/idf_py_stdout_output_21208 and idf_py_stdout_output_4624 | Recorded size-tool output supporting the B1 section breakdown, capacities and percentages |
| C1 | [main/main.c](../src/esp32-udp-client/main/main.c), [sdkconfig.defaults](../src/esp32-udp-client/sdkconfig.defaults), [FUNCTION_REGISTRY.md](FUNCTION_REGISTRY.md), [VARIABLE_REGISTRY.md](VARIABLE_REGISTRY.md) | Measurement API locations, scope, constants, buffers and configuration |

All build/ log paths in this document are relative to phase1-udp-baseline/src/esp32-udp-client/. H1 remains the primary committed source for the successful runtime results. Supplemental existing capture values are identified separately below. No credential values are included.

## 1. Hardware and software baseline

| Parameter | Recorded configuration / identification | Source |
| --- | --- | --- |
| Module / chip | ESP32-S3-WROOM-1 module identification supplied by user; chip independently identified as ESP32-S3 QFN56, revision v0.2 | H1 and preceding G4 hardware attempt |
| Flash capacity | 16 MB as reported by identification; this is physical capacity, not application allocation | H1; completion report hardware baseline |
| Physical PSRAM | 8 MB; embedded AP_3v3 identification | Existing G4 hardware identification |
| PSRAM use | Disabled; CONFIG_SPIRAM unset | B2, C1; applies to all baseline resource readings |
| Crystal | 40 MHz | Hardware identification, B2 |
| Flash operation | QIO selected and observed; 40 MHz configured | B2, H1 spi_flash log |
| Boot header | SDK flasher uses DIO initially, bootloader enables QIO; these refer to different stages | B1/H1 and completion report |
| CPU / optimization configuration | CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ=160; CONFIG_COMPILER_OPTIMIZATION_DEBUG=y; CONFIG_PM_ENABLE unset | B2 effective configuration; configuration values, not measured CPU utilization/clock tracing |
| SDK | ESP-IDF v5.5.4; commit 735507283d5b2f9fb363a1901172dbd9e847945d; 27 pinned submodules | B1/B2 |
| Language / execution | C with FreeRTOS; existing main task plus SDK Wi-Fi/network/event tasks; no custom application task | C1 |
| Toolchain | GCC 14.2.0, Espressif esp-14.2.0_20260121; Python 3.11.2; CMake 3.30.2; Ninja 1.12.1; esptool v4.12.dev2 | B1 |
| Host / server | Windows NT 10.0.22631.0 x64, PowerShell; Node.js v25.9.0, built-in dgram | Recorded environment |
| FreeRTOS ticks | CONFIG_FREERTOS_HZ=100 | B2; not a CPU-frequency measurement |
| Main task stack | CONFIG_ESP_MAIN_TASK_STACK_SIZE=4096 bytes | B2/C1 |
| Console / port | UART 115200 baud; COM14 | B2/H1 |
| Partition layout | Single factory app: offset 0x10000, size 0x100000 (1048576 bytes); NVS 0x9000/24K, phy_init 0xf000/4K; partition table 0x8000 | Existing G4 flash/partition evidence; B2/C1 |
| Wi-Fi buffers | Configured static RX count 10, dynamic RX limit 32, dynamic TX limit 32 | B2; counts are configuration, not observed allocation peaks |
| UDP configuration | One IPv4 peer 192.168.0.170:56830; at most 20 attempted exchanges; 64-byte receive buffer | C1/H1 |
| Credentials | Local ignored .private/wifi_credentials.h; case correction supplied by user; values intentionally excluded | H1/C1 |
| Target security during successful runs | Flash encryption disabled after explicitly approved counter change to 0b011, erase and reflash; Secure Boot disabled; existing permanently disabled JTAG unchanged | H1; no security change made for this documentation task |

### Commit and image traceability

Branch: phase1-udp-baseline. Documentation input commit: 7d865055ed32cb7b09eb2457a6aef9a4ea9cc492. Accepted tracked ESP32 source/project files last changed in e7de4f70d1feb93853fcd6b78f72b3e89d4dfe8d. G4 evidence/recovery is recorded in b13620fe2b75084bd31fe5869da57c00beef8049 and consolidated by 7d86505. Later documentation commits are not firmware rebuilds.

| Image / artifact | Recorded SHA-256 | Identification / use |
| --- | --- | --- |
| B1 G3 application image | 2ce56259093cadec76946ad5d3d5226634e5a894e02618e9209956b1af2e3af5 | 757920 bytes; app metadata c2b2b7a-dirty; selected source for the detailed build-section report, before corrected private network configuration |
| Initial G4 application image | 35f77f2325b7dc54cd395f4f3abae5d9436a0be452ceacae5d38d8eb299ffb88 | 757920 bytes; pre-correction image; initial ROM boot failure then private-network issue; not the R1/R2 image |
| Final tested R1/R2 application image | 594f9b9ef0d0abc3d5ddc387f263380254bc862c25426ee3539a8334c1f8d762 | 757920 bytes; app metadata c10418f-dirty; corrected private configuration; use this as final runtime baseline image identity |
| Accepted main.c | f5e306b74759f66a02463ba552afcfe570e09c9e7b45fd7017564486aa6bf7a5 | Same tracked firmware source in B1 and final hardware runs |
| Retained bootloader | d161ddc1b6a6846cf9bd68d76d27cdd2cb1d70e8a5f48b769dfb6a56024ac76d | 22272 bytes; B1/B2 artifact identity |
| Effective sdkconfig | 03e46ff2ab04834743ee18e0dd63223202d9a3672f5a1ae2ede63049e8e4a9d2 | Complete non-secret snapshot preserved in B2; private credentials supplied separately |

Different image hashes are not contradictory measurements: source/build metadata and private inputs identify different images. Exact byte-for-byte reproduction is not established merely by matching source and sdkconfig; credentials, timestamps, app metadata and tool environment must match. Credential-bearing binaries/ELF remain local and untracked. Future comparisons must retain their own image identity, not overwrite these hashes.

## 2. Flash resource usage — build evidence

B1/L1 values are recorded build measurements. The final R1/R2 binary size is independently preserved in H1, but no separate final-image size-section report is recorded. Do not silently relabel the earlier section totals as measurements of the final image.

| Metric / exact size-tool classification | Value | Unit | Provenance / scope |
| --- | ---: | --- | --- |
| Final R1/R2 application .bin | 757920 | bytes | H1 final artifact |
| Bootloader .bin | 22272 | bytes | B1/B2 |
| Factory application partition | 1048576 (0x100000) | bytes | B1, configured allocation |
| Unused application partition capacity | 290656 (0x46f60) | bytes | Recorded B1 build fit check; final binary has same recorded length |
| Free partition percentage, tool rounded | 28 | % | Recorded fit check; not percentage of entire 16 MB flash |
| Size-tool total image, before binary padding | 757797 | bytes | B1; differs from .bin file length |
| Flash Code aggregate | 539766 | bytes | B1/L1 |
| Flash Code .text | 539766 | bytes | L1; child of Flash Code, not additional storage |
| Flash Data aggregate | 107072 | bytes | B1/L1 |
| Flash Data .rodata | 106816 | bytes | L1; child of Flash Data |
| Flash Data .appdesc | 256 | bytes | L1; child of Flash Data |

Application file size, partition allocation and physical flash capacity are distinct. Do not add parent categories to their children or attribute the complete firmware/bootloader to UDP. Total unallocated device-wide flash is **Not Measured**; no estimate is derived from the nominal 16 MB capacity.

## 3. Static RAM — exact size-tool region classifications

Source B1/L1. These linker-region usage values include code where .text/.vectors are listed; they are not all static variable storage and are not runtime heap readings.

| Region / section | Used bytes | Tool-reported used % | Remaining bytes | Region total bytes |
| --- | ---: | ---: | ---: | ---: |
| DIRAM aggregate | 110135 | 32.23 | 231625 | 341760 |
| DIRAM .text | 74579 | 21.82 | — | — |
| DIRAM .data | 19964 | 5.84 | — | — |
| DIRAM .bss | 15592 | 4.56 | — | — |
| IRAM aggregate | 16384 | 100.0 | 0 | 16384 |
| IRAM .text | 15356 | 93.73 | — | — |
| IRAM .vectors | 1028 | 6.27 | — | — |
| RTC SLOW aggregate | 32 | 0.39 | 8160 | 8192 |
| RTC SLOW .force_slow | 32 | 0.39 | — | — |
| RTC FAST aggregate | 24 | 0.29 | 8168 | 8192 |
| RTC FAST .rtc_reserved | 24 | 0.29 | — | — |

A dash means no separate section-level remaining/total value was reported, not zero. DIRAM .data + .bss = **35556 bytes (Derived)**, a subtotal already included in DIRAM. No combined IRAM+DIRAM+heap total is calculated: address aliases, linker regions, code/data classifications and runtime allocation pools must not be treated as additive independent capacities. The reported 100% IRAM value applies to the specific 16384-byte linker region, not all chip RAM.

## 4. Runtime heap — two successful hardware runs

Source H1/R1/R2. All values are bytes returned for **MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT**. They are free internal 8-bit-capable heap, not total RAM consumption or exclusively UDP-owned memory. PSRAM was disabled.

| Checkpoint / API | R1 baseline bytes | R2 reset/completion bytes | Timing / interpretation |
| --- | ---: | ---: | --- |
| Before Wi-Fi initialization: heap_caps_get_free_size | 329724 | 329724 | Startup checkpoint, before NVS/network/Wi-Fi setup in app_main |
| After successful IPv4 acquisition: heap_caps_get_free_size | 278212 | 278200 | After Wi-Fi/IP, before UDP socket creation |
| Result after exchanges, before cleanup: heap_caps_get_free_size | 277576 | 277576 | Before socket close, Wi-Fi stop or deinit |
| API-reported minimum as of result: heap_caps_get_minimum_free_size | 275460 | 274008 | Low-water mark not reset around UDP; includes startup/Wi-Fi/platform activity, not a UDP-specific allocation peak |
| After cleanup | Not Measured | Not Measured | Return-from-main logs do not supply a heap reading |

Do not infer UDP heap peaks by subtracting these checkpoints or by treating the SDK watermark as an isolated UDP measurement. Concurrent SDK activity and allocator history are included.

### Supplemental existing negative/setup-run readings

These are retained observations from existing local sanitized captures, not substitutes for R1/R2 or additional successful baseline runs. All below are bytes; before-Wi-Fi free heap was 329724 in every listed capture. "Not Measured" after IPv4 means that checkpoint was not reached, not zero free heap. All result readings precede cleanup. The first supplemental row belongs to the initial G4 image (35f77f...); the remaining rows use the final corrected image (594f9b...). Full hashes are in the identity table.

| Existing local capture in build/ | Setup | After IPv4 free | At-result free | API minimum at result | Main-stack minimum free |
| --- | --- | ---: | ---: | ---: | ---: |
| g4-baseline-retest-sanitized.log | Initial private SSID mismatch, reason 201 | Not Measured | 279572 | 279308 | 1748 |
| g4-server-absent-sanitized.log | Association setup failure, reason 4, zero UDP attempts | Not Measured | 279308 | 277548 | 1748 |
| g4-server-absent-run2-sanitized.log | Qualifying server-absent run | 278200 | 277648 | 275456 | 1748 |
| g4-negative-FAIL-sanitized.log | Four-byte nonmatching response | 278200 | 277648 | 275456 | 1748 |
| g4-negative-LF-sanitized.log | PONG plus newline | 278164 | 277648 | 275428 | 1748 |
| g4-negative-OVERSIZE-sanitized.log | 100-byte response, receive capped at 64 | 278164 | 277648 | 275456 | 1748 |
| g4-negative-WRONGPORT-sanitized.log | PONG from incorrect UDP source port | 278200 | 277612 | 275388 | 1748 |

The original ROM invalid-header phase never reached application measurement checkpoints; application resource readings for that failure are **Not Measured**. H1 and the earlier G4 attempt retain those failure observations.

## 5. Main-task stack

Source B2/C1 for configured size; H1/R1/R2 for readings. The logged uxTaskGetStackHighWaterMark(NULL) values are recorded in bytes under this ESP-IDF baseline.

| Parameter | R1 bytes | R2 bytes | Type |
| --- | ---: | ---: | --- |
| Configured main stack | 4096 | 4096 | Recorded configuration, not measured use |
| Observed minimum free main stack | 1748 | 1748 | Runtime measurement, as of pre-cleanup result log |
| Derived maximum observed occupancy at that checkpoint | 2348 | 2348 | Derived: 4096 - 1748 |

The high-water mark is an observed minimum-free-stack reading. The derived occupancy is not a guaranteed worst-case stack requirement, and the checkpoint precedes cleanup. SDK Wi-Fi, event-loop and TCP/IP task stack usage and main-stack low-water mark after cleanup are **Not Measured**.

## 6. Network and processing observations

| Item | R1 | R2 | Type / source |
| --- | --- | --- | --- |
| Attempted / successful exchanges | 20 / 20 | 20 / 20 | Runtime counters, H1 |
| Positive-run receive timeouts | 0 | 0 | Existing successful run logs, H1 |
| Request / expected response payload | 4 / 4 bytes | 4 / 4 bytes | Configured PING/PONG; observed lengths, C1/H1 |
| Application payload sent / successfully received | 80 / 80 bytes | 80 / 80 bytes | Derived: 20 x 4 each direction; excludes all transport/link overhead |
| Configured inter-exchange delay | 1000 ms | 1000 ms | C1; delay after success, not a fixed send period or measured latency |
| Configured receive timeout | 2 seconds | 2 seconds | C1; maximum wait setting, not measured RTT |
| First app trial-log timestamp | 406 ms | 406 ms | Existing R1/R2 logs |
| IPv4-acquired log timestamp | 2886 ms | 2206 ms | H1/R1/R2 |
| Result-log timestamp | 22096 ms | 21426 ms | H1/R1/R2; raw log clock values, not durations by themselves |
| SDK return-from-main timestamp | 22176 ms | 21506 ms | Existing R1/R2 logs; after cleanup path |
| First app trial log to result span | 21690 ms | 21020 ms | Derived: result timestamp - 406; includes setup and configured delays |
| IPv4-acquired log to result span | 19210 ms | 19220 ms | Derived from the two corresponding log timestamps |
| First app trial log to SDK return span | 21770 ms | 21100 ms | Derived; observed app-log window including cleanup, excludes earlier boot/startup |

These timestamp differences are observed log-window durations, not instrumented CPU processing time, complete power-on-to-shutdown duration or isolated UDP execution time. Exact total reset-to-end trial duration, per-exchange RTT statistics, CPU utilization/time, actual CPU clock trace, throughput benchmark, UDP/IP/Wi-Fi full on-wire size, retransmissions at Wi-Fi level and energy/power are **Not Measured**. No overhead is estimated. The NodeJS 21/21 localhost results are separate server validation and are not added to ESP32 resource-run counts.

## 7. Measurement conditions and comparability

- R1 and R2 were obtained on 2026-11-11 using the final image and the same tracked main.c, SDK, build settings and board described above. Exactly two successful resource-baseline runs are available; additional negative/setup runs are identified separately.
- One app-owned IPv4 UDP socket, a 64-byte receive buffer, four-byte protocol arrays and one event group. SDK networking/event tasks and allocations are included in firmware/heap observations; no additional app task. Functions/API ownership and variables are documented in C1 registries.
- Hardware/network: private Wi-Fi network, PC-side inspection recorded 2.4 GHz/channel 4, server 192.168.0.170:56830. ESP32 acquired 192.168.0.122. RSSI, RF interference, AP load and link-layer traffic/PHY statistics were not recorded as resource measurements. getsockname logged wildcard 0.0.0.0 with matching ephemeral ports; cloud and acquired-IP logs establish the actual source.
- PSRAM disabled; no claim that enabling it would preserve this baseline. Do not mix external RAM with these internal-capability heap readings.
- API checkpoints are before Wi-Fi setup, after IPv4, and after exchanges before teardown. Heap minimum is the API's accumulated watermark as of the result checkpoint. Stack reading is for the calling main task before teardown. No post-cleanup resource checkpoint exists.
- Wi-Fi/NVS initialization, DHCP, driver buffers, SDK logging and other system work contribute to the observations. The entire binary and networking memory are not attributable exclusively to UDP.
- Board recovery and private network correction are already approved historical deployment conditions, not newly performed actions. AP-off tests S2-T11/T12 remain unexecuted at user direction; do not count the initial SSID-mismatch failure as those controlled tests.

## 8. Standardized baseline comparison table

R1/R2 pairs retain both observations rather than selecting one as a guarantee. Types distinguish recorded configuration/specification from measured use. Source IDs above define exact populations, image identity and checkpoints.

| Parameter | Phase 1 Raw UDP | Unit | Measurement Type | Notes |
| --- | --- | --- | --- | --- |
| Physical flash capacity | 16 | MB (as reported) | Recorded specification | Not application allocation |
| Physical PSRAM capacity | 8 | MB (as reported) | Recorded specification | Physical presence, not an enabled heap pool |
| PSRAM use | disabled | status | Recorded configuration | Disabled for all baseline measurements |
| Default CPU clock | 160 | MHz | Recorded configuration | Not a measured clock trace |
| FreeRTOS tick rate | 100 | Hz | Recorded configuration | Same timing base required for comparison |
| Application binary, final image | 757920 | bytes | Build measurement | H1 final hash; full firmware, not UDP-only |
| Bootloader binary | 22272 | bytes | Build measurement | B1/B2 |
| Application partition | 1048576 | bytes | Recorded configuration | 0x100000, 1 MiB |
| Unused application partition | 290656 | bytes | Build measurement | Recorded fit check; rounded free percentage 28% |
| Size-tool image before padding | 757797 | bytes | Build measurement | B1 image, not separately remeasured final sections |
| Flash Code .text | 539766 | bytes | Build measurement | B1/L1 |
| Flash Data aggregate | 107072 | bytes | Build measurement | Includes .rodata and .appdesc |
| Flash Data .rodata | 106816 | bytes | Build measurement | Child of aggregate; do not add twice |
| Flash Data .appdesc | 256 | bytes | Build measurement | Child of aggregate |
| DIRAM aggregate | 110135 | bytes | Build measurement | B1/L1; includes code/data/bss |
| DIRAM .text | 74579 | bytes | Build measurement | Included in DIRAM |
| DIRAM .data | 19964 | bytes | Build measurement | Included in DIRAM |
| DIRAM .bss | 15592 | bytes | Build measurement | Included in DIRAM |
| DIRAM .data + .bss subtotal | 35556 | bytes | Derived | Included in DIRAM; no cross-region sum |
| IRAM aggregate | 16384 | bytes | Build measurement | Specific reported 16384-byte region |
| IRAM .text | 15356 | bytes | Build measurement | Child of IRAM aggregate |
| IRAM .vectors | 1028 | bytes | Build measurement | Child of IRAM aggregate |
| RTC SLOW .force_slow | 32 | bytes | Build measurement | Reported RTC SLOW usage |
| RTC FAST .rtc_reserved | 24 | bytes | Build measurement | Reported RTC FAST usage |
| Internal 8-bit heap free before Wi-Fi | R1 329724; R2 329724 | bytes | Runtime measurement | Startup checkpoint |
| Internal 8-bit heap free after IPv4 | R1 278212; R2 278200 | bytes | Runtime measurement | Before socket creation |
| Internal 8-bit heap free after exchanges | R1 277576; R2 277576 | bytes | Runtime measurement | Before cleanup |
| Internal 8-bit heap API minimum at result | R1 275460; R2 274008 | bytes | Runtime measurement | Watermark includes SDK/Wi-Fi setup |
| Internal 8-bit heap free after cleanup | Not Measured | bytes | Not measured | No checkpoint |
| Main-task configured stack | 4096 | bytes | Recorded configuration | Same setting required for comparison |
| Main-task minimum free stack | R1 1748; R2 1748 | bytes | Runtime measurement | Pre-cleanup reading |
| Main-task derived maximum observed occupancy | R1 2348; R2 2348 | bytes | Derived | Configured size minus minimum free |
| Attempted exchanges | R1 20; R2 20 | count | Runtime measurement | Two positive runs only |
| Successful exchanges | R1 20; R2 20 | count | Runtime measurement | Same run population as attempts |
| Request payload | 4 | bytes | Runtime measurement | Exact PING; not a full packet |
| Response payload | 4 | bytes | Runtime measurement | Exact PONG; not a full packet |
| Payload bytes sent per positive run | 80 | bytes | Derived | 20 x 4; excludes all overhead |
| Valid payload bytes received per positive run | 80 | bytes | Derived | 20 x 4; excludes all overhead |
| Configured exchange interval | 1000 | ms | Recorded configuration | vTaskDelay after success |
| Configured receive timeout | 2 | seconds | Recorded configuration | SO_RCVTIMEO |
| App-start-log to result span | R1 21690; R2 21020 | ms | Derived | Log-window duration, not CPU time |
| IPv4-log to result span | R1 19210; R2 19220 | ms | Derived | Includes deliberate pacing |
| App-start-log to SDK-return span | R1 21770; R2 21100 | ms | Derived | Includes cleanup; excludes preceding boot |
| Exact total reset-to-end duration | Not Measured | ms | Not measured | No defined full-duration instrument |
| Per-exchange RTT distribution | Not Measured | ms | Not measured | No RTT instrumentation |
| CPU utilization | Not Measured | % | Not measured | No CPU profiler |
| CPU processing time | Not Measured | ms | Not measured | Elapsed log spans are not CPU time |
| UDP-specific incremental flash | Not Measured | bytes | Not measured | No controlled subtraction baseline |
| UDP-specific incremental RAM | Not Measured | bytes | Not measured | No attributable isolation |
| UDP-specific dynamic allocation peak | Not Measured | bytes | Not measured | Shared heap watermark cannot isolate it |
| Other SDK task stack peaks | Not Measured | bytes | Not measured | Only main-task reading available |
| Complete on-wire packet size | Not Measured | bytes | Not measured | No packet capture/link-layer measurement |
| Measured throughput | Not Measured | bytes/second | Not measured | Pacing trial is not throughput benchmark |
| Power consumption | Not Measured | mW | Not measured | No power instrument |
| Energy consumption | Not Measured | mJ | Not measured | No energy instrument |
| Guaranteed worst-case RAM/stack | Not Measured | bytes | Not measured | Two observed positive runs are insufficient |

## 9. Limitations, evidence differences and measurement gaps

1. No CPU utilization, isolated protocol processing time, protocol-specific incremental flash/RAM, UDP-attributable heap peak, post-cleanup heap, complete packet size, power or wider statistical/worst-case measurements exist. Missing measurements are not zero. Further instrumentation would require separate approval; none is implemented here.
2. Size-section rows are explicitly B1/L1, while positive runtime rows are from the later final hash in H1. An exact final-image section report was not recorded. Identical recorded binary lengths do not prove identical section layouts.
3. Earlier local size output idf_py_stdout_output_31648 reports Flash Code 539758, whereas the selected later records report 539766. Preserve these as different intermediate builds; do not average or substitute the earlier number. This document selects the final-source B1/L1 report and retains the earlier historical evidence unchanged. No conflicting R1/R2 reading was found; expected image/build provenance differences are disclosed.
4. Original image, pre-correction G4 image and final tested image have different SHA-256 values. Firmware metadata and credentials are private/build inputs; a source commit alone cannot reproduce the exact tested binary. Full effective configuration and hashes are preserved in B2, but binary/ELF and credentials are deliberately not public Git artifacts.
5. No peak simultaneous whole-chip RAM use or reliable UDP-only allocation peak is derived by adding linker regions, heap observations and task stacks. The 35556-byte data/bss subtotal and 2348-byte stack occupancy are explicitly derived, not new readings.
6. AP-off tests were intentionally unexecuted. Tick rollover/full 30-second expiry, allocator/unregister/deinit failure paths and SDK constructor assertions lack runtime fault-injection coverage. An association setup failure and prior ROM boot failure are retained; two positive runs do not establish reliability under arbitrary network or resource conditions.
7. The measurement checkpoint precedes cleanup. Cleanup logs show main returned without cleanup warnings, but they do not measure memory returned afterward. Intentionally stopping Wi-Fi emits disconnect reason=8 after PASS; that is not a failed exchange.

## 10. Future CoAP comparison requirements

For each comparable row use **CoAP measurement - Raw UDP measurement**, preserving the same unit, capability pool, section classification, checkpoint definition and run identification. A difference is a derived value and must retain both input measurements and their sources.

Match or explicitly document differences in chip/revision, flash/PSRAM configuration, SDK/toolchain, optimization/default CPU clock, tick rate, partition, main-task stack, Wi-Fi buffers, logging/instrumentation, AP conditions, message semantics, exchange count, pacing and timeout. Preserve both per-run values or a separately justified statistical protocol; do not choose one UDP run merely to favor an outcome. Changed payloads must have their lengths recorded.

Compare whole application .bin to whole application .bin and matching size-tool sections to matching sections. Such deltas are whole-build deltas unless a controlled experiment isolates protocol changes. They are not automatically the incremental footprint of UDP or CoAP. Do not subtract free heap from unrelated checkpoints or compare an internal-only pool with total/external RAM. A lower free-heap value is not the same sign convention as larger allocated usage; report the free-heap delta explicitly.

For a row marked Not Measured, do not compute a numeric delta. Newly required instrumentation, post-cleanup checkpoints, CPU profiling, packet capture or power measurements are future measurement gaps only and require the project development gates/approval. No inference about preferred long-term architecture is made.
