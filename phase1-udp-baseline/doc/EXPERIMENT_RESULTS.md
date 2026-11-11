# Phase 1 completion report — Step 3 review candidate (2026-11-11)

Status: documentation consolidation complete; independent review and user acceptance pending. This is a candidate milestone baseline, not a declaration of Phase 1 acceptance. No source development, SDK modification, hardware action, build or test rerun was performed in Step 3. The historical records below are retained in their original chronological order.

## Repository and reviewed implementation

Audit input: branch phase1-udp-baseline, commit b13620fe2b75084bd31fe5869da57c00beef8049, synchronized with origin and initially clean. The resulting Step 3 commit changes documentation only; its identifier is in Git history and the handoff. No milestone tag or merge.

- NodeJS: phase1-udp-baseline/src/cloud.js, last source change ca864c9a0e849e7d0fa443c736ace06b78aa467f; user reviewed and accepted.
- ESP32: phase1-udp-baseline/src/esp32-udp-client/main/main.c and its two CMakeLists.txt files plus sdkconfig.defaults, last source changes e7de4f70d1feb93853fcd6b78f72b3e89d4dfe8d; G3 source review accepted.
- Raw IPv4 UDP only: exact four-byte PING/PONG, reply to source endpoint, basic logging/errors. Firmware has two custom functions, one application-owned socket, no custom task or reconnect/retry mechanism, and at most 20 attempted exchanges per boot. NodeJS remains continuously listening until terminated/error.

FILE_REGISTRY.md, FUNCTION_REGISTRY.md, VARIABLE_REGISTRY.md and PROTOCOL_REGISTRY.md were checked against committed implementation. All 21 existing tracked files are covered. Corrected a stale pending-G2 label on the historical diagram entry and scoped the function-registry introductory statement explicitly to Step 1. No source mismatch was found. Final G2 details, dependencies, variables and approval history are cross-referenced and consolidated in project-doc/ARCHITECTURE_DECISIONS.md.

## Hardware, software and build identity

Verified target: ESP32-S3-WROOM-1 (module marking user-supplied), ESP32-S3 revision v0.2, 16 MB Quad flash, physical 8 MB PSRAM, 40 MHz crystal, COM14. Built with PSRAM disabled, QIO selected, 40 MHz flash, 4096-byte main stack, 100 Hz FreeRTOS ticks, UART 115200 and the default 1 MiB factory application partition. SDK flasher uses a DIO boot header before bootloader QIO initialization; observed application flash I/O was QIO.

Windows NT 10.0.22631.0 x64, PowerShell, Node.js v25.9.0. ESP-IDF v5.5.4, commit 735507283d5b2f9fb363a1901172dbd9e847945d; 27 pinned submodules; Python 3.11.2; GCC 14.2.0, esp-14.2.0_20260121; CMake 3.30.2; Ninja 1.12.1; esptool v4.12.dev2. Tool versions are preserved from the build/test record; Step 3 did not execute a build or hardware tool. The SDK's tracked diff remains empty on filesystem audit; unrelated pre-existing untracked SDK projects remain outside the build inputs.

Destination: 192.168.0.170:56830 over the privately supplied 2.4 GHz network. Credentials remain in an ignored local header; no values appear here. Final application image is 757920 bytes, SHA-256:

```text
594f9b9ef0d0abc3d5ddc387f263380254bc862c25426ee3539a8334c1f8d762
```

Final image metadata is c10418f-dirty because documentation edits were pending when it was built. This does not mean the SDK or accepted source changed. Exact binary reproduction is not claimed: credentials, build metadata/timestamps and tool environments must also match. Firmware/ELF are retained locally, not published in Git because they contain credentials. Hash verification of retained artifacts in Step 3 is filesystem inspection only.

## Consolidated exchange and negative-test evidence

| Scope | Existing observed results | Qualification |
| --- | --- | --- |
| Step 1 positive localhost | 20 sequential exchanges plus one liveness exchange: 21/21 exact PONG | No response timeouts or assertion failures in these positive checks |
| Step 1 unknown payloads | Five unknown/near-match datagrams logged; zero replies within 500 ms each | Expected observation timeouts, not positive-run packet loss |
| Step 1 other checks | T01-T08 passed; occupied port reported EADDRINUSE with exit 1 | T07 passed by user-reported manual Ctrl+C verification; original automated attempt remains inconclusive historically |
| Step 2 positive hardware | Two independent runs of 20 attempts/20 successes: 40/40 total | Cloud confirmed sender-directed replies; zero positive-run timeouts |
| Step 2 negative socket checks | Absent server, FAIL, PONG+LF, 100-byte response and wrong-source-port PONG | Five independent one-attempt runs; no accepted replies; two expected timeouts and three rejections; no automatic retries |
| Step 2 suite | 12 PASS; 2 NOT EXECUTED by user direction | Includes one static-inspection test, not 12 independent runtime tests |

No aggregate loss rate combines intentional failures with positive runs. Earlier ROM boot failure, initial private SSID mismatch and one association setup failure (reason=4, zero UDP attempts before the qualifying absent-server run) are retained. No further exchange or negative test was performed for Step 3.

## Firmware resource evidence

All values below are copied from existing build/runtime evidence, not newly measured on hardware.

| Measurement | Existing observed value | Scope / limitation |
| --- | --- | --- |
| Final application .bin | 757920 bytes | Retained tested image; 28% of 1 MiB application partition remains free |
| Bootloader .bin | 22272 bytes | Retained bootloader artifact |
| Size-tool image before binary padding | 757797 bytes | G3 final-source build report; private configuration subsequently case-corrected; final binary length unchanged |
| DIRAM used | 110135 bytes | Build report: text 74579, data 19964, bss 15592 |
| Static data + bss within DIRAM | 35556 bytes | Sum of the two reported sections; excludes code and dynamic allocations |
| Flash code / flash data | 539766 / 107072 bytes | Existing build report, not an architecture comparison |
| IRAM region used / reported region size | 16384 / 16384 bytes | Specific linker region; not a claim that all chip IRAM is exhausted |
| RTC slow / RTC fast sections | 32 / 24 bytes | Build report |
| Internal heap free before Wi-Fi | 329724 / 329724 bytes | Baseline / reset-completion positive runs |
| Internal heap free after IPv4 | 278212 / 278200 bytes | Same two positive runs |
| Internal heap free at result | 277576 / 277576 bytes | Before teardown, not post-cleanup free heap |
| Minimum internal heap by result | 275460 / 274008 bytes | Observed minima; not worst-case guarantees |
| Main-stack minimum free | 1748 / 1748 bytes | Observed high-water-mark readings with 4096-byte configured main stack |

## Approved exceptions and decisions

The approved final G2 uses app_main as sole connection owner, STARTED synchronization, one shared 30-second unsigned-tick deadline, latched FAILED, exact peer/response validation, and callback-safe conditional event-group deletion. Reuse of restored ESP-IDF 5.5.4 replaced the earlier proposed isolated installation. Private SSID case correction was user-supplied, made only in the ignored header, then rebuilt/tested without tracked source changes.

After discovering flash encryption on the original target, the user explicitly authorized SPI_BOOT_CRYPT_CNT 0b001 -> 0b011, full-chip erase and reflash. Readback reported encryption disabled; existing key purpose and permanent JTAG disable remained. That was a deployment exception, not a new firmware security framework. ADR-001/003/004/005 record the approved choices; ADR-002 remains superseded. No architecture choice is invented by closure and no security operation was repeated in Step 3.

## Unexecuted tests, limitations and unresolved matters

- S2-T11 (AP unavailable before boot) and S2-T12 (AP disabled mid-run) remain unexecuted at explicit user direction. Initial NO_AP_FOUND evidence is retained but is not substituted for the waived controlled AP tests.
- NodeJS send-error and non-bind socket-runtime-error branches were not fault-injected. Firmware send/setup/allocation/unregister/deinit failure branches were not exhaustively fault-injected. S2-T14 is static inspection only; the SDK constructor's internal assertion behavior remains untested.
- Shared 30-second full-deadline expiry and FreeRTOS tick-counter rollover were not exercised on hardware; deadline arithmetic was statically reviewed.
- Step 1 manual Ctrl+C verification lacks a supplied transcript, exact manual-test date/runtime/platform. Its evidence remains user-reported.
- One association failure with reason=4 was observed on the corrected configuration before an absent-server run. Its root cause was not established; the later independent run passed. The two positive runs do not establish broader network reliability, long-run stability, concurrency or performance bounds.
- getsockname reports wildcard 0.0.0.0; acquired IPv4 and cloud logs establish the source address. Intentional Wi-Fi shutdown logs disconnect reason=8 at error level after PASS. These logging characteristics remain unchanged.
- The committed PNG is a historical proposal diagram and omits final startup/cleanup revisions. The consolidated approved PlantUML record in ARCHITECTURE_DECISIONS.md is the final textual design reference; no new PNG was rendered.
- Full non-secret effective configuration and pinned submodule list are archived below. Exact firmware binary reproduction remains limited by deliberately unpublished credentials and build metadata. No permanent test-client files or automated replay suite exist.
- Source reviews are accepted; final Phase 1 acceptance is still the user's decision. There is no unresolved failing positive UDP test in the final qualifying evidence, but the coverage gaps and historical setup failures above remain explicit.

## Recommendation for independent acceptance review

Recommend Phase 1 acceptance as a bounded UDP request/response experimental baseline, conditional on the reviewer/user accepting the two explicitly unexecuted AP tests and the listed evidence limitations. This recommendation is not acceptance, authorization for a tag/merge, or a conclusion about long-term UDP versus CoAP architecture. Phase 2 has not started. An annotated phase1-udp-baseline-v1.0 tag may be created only after acceptance and a separate explicit authorization.

Source/artifact identities and complete build-configuration archive are appended after the historical observations below. For past per-test setups, expected results, actual logs and statuses, retain the Step 1 and Step 2 sections that follow.

---

# Step 1 experiment results

## Environment

Date: 2026-11-10 (Asia/Calcutta). Windows NT 10.0.22631.0, Node.js v25.9.0, win32 x64, PowerShell. Branch: phase1-udp-baseline. Starting commit: af4b50ef48305e17bdd77d4c899f0dbec1493cb5.

## Execution procedure

From the repository root:

1. Check Node.js: `node --version`.
2. Optional syntax check: `node --check phase1-udp-baseline/src/cloud.js`.
3. Start: `node phase1-udp-baseline/src/cloud.js`.
4. Confirm `Listening on UDP 0.0.0.0:56830`. Change UDP_PORT in the source to configure another port; update the client destination too. No compilation or package manager is required.
5. In a second PowerShell terminal, run this temporary single-request client with a 1000 ms timeout and no retries:

```powershell
@'
const client = require('node:dgram').createSocket('udp4');
const timer = setTimeout(() => {
  console.error('Timeout after 1000 ms');
  process.exitCode = 1;
  client.close();
}, 1000);
client.on('error', error => {
  clearTimeout(timer);
  console.error(error);
  process.exitCode = 1;
  client.close();
});
client.once('message', (data, sender) => {
  clearTimeout(timer);
  console.log('Local endpoint:', client.address());
  console.log('Response:', JSON.stringify(data.toString('utf8')), sender);
  if (!data.equals(Buffer.from('PONG', 'ascii'))) process.exitCode = 1;
  client.close();
});
client.send(Buffer.from('PING', 'ascii'), 56830, '127.0.0.1');
'@ | node
```

6. Observe exact PONG and compare the client endpoint with the server's reply destination. Stop with Ctrl+C in the server terminal. Manual Ctrl+C termination was subsequently verified successfully by the user, as recorded below.

## Executed tests

A temporary command-line orchestration client used a bound localhost socket, exact Buffer assertions, 500 ms per-packet observation timeouts and no retries. No test script or output file was retained in the repository.

| Test | Actual observation | Result |
| --- | --- | --- |
| Syntax | node --check exited 0 | Passed |
| T01 | Bound 0.0.0.0:56830 | Passed |
| T02 | Received/logged PING from 127.0.0.1:54700, bytes=4 | Passed |
| T03 | Exact four-byte PONG received | Passed |
| T04 | Client endpoint 127.0.0.1:54700 matched logged reply destination; response source 127.0.0.1:56830 | Passed |
| T05 | 20 sequential exchanges; 20 successes, zero response timeouts or assertion failures | Passed |
| T06 | UNKNOWN, ping, PING+LF, PING+NUL, PING+0xff logged; no reply within 500 ms each | Passed |
| T07 | Original tool-injected Ctrl+C attempt was inconclusive; user subsequently reported successful manual Ctrl+C termination | Passed (user-reported manual verification) |
| T08 | Second server exited 1 with EADDRINUSE; original server answered another PING | Passed |

Recognized traffic: 21 requests, 21 exact responses, zero response timeouts or assertion failures. Unknown traffic: five requests, zero responses, five expected observation timeouts. A separate startup was used for T07. After Ctrl+C injection, a bind probe reported EADDRINUSE; the process was explicitly stopped and a fresh bind confirmed the port was reusable. Forced cleanup is not evidence of normal Ctrl+C termination.

## Manual verification and source review

The user subsequently confirmed that T07 (Ctrl+C termination) was manually verified successfully and that cloud.js is acceptable without source changes. This is user-reported evidence, not a newly executed agent test. The manual verification date, platform, Node.js version and console transcript were not supplied. The original automated attempt and cleanup observations above are retained as historical evidence.

## Representative captured logs

```text
Listening on UDP 0.0.0.0:56830
Client endpoint { address: '127.0.0.1', family: 'IPv4', port: 54700 }
Received from 127.0.0.1:54700 bytes=4 payload="PING"
Sent PONG to 127.0.0.1:54700
Received from 127.0.0.1:54700 bytes=7 payload="UNKNOWN"
Received from 127.0.0.1:54700 bytes=4 payload="ping"
UDP socket error: EADDRINUSE: bind EADDRINUSE 0.0.0.0:56830
Cleanup confirmed: port 56830 reusable
```

## Limitations

Only localhost was tested. Send-error and non-bind runtime-error branches were implemented but not fault-injected. The original automated T07 attempt did not establish Ctrl+C delivery; the subsequent user-reported manual T07 passed. Process/network metadata queries were restricted. No LAN, embedded, CoAP, security or broader reliability conclusions follow. Step 1 source review is accepted; complete Phase 1 milestone acceptance remains pending later work and review.


# Phase 1, Step 2 — G3 implementation and build evidence

Date: 2026-11-11, Asia/Calcutta. Branch: phase1-udp-baseline. G1 and final G2 approved before source creation. G3 build authorized; G4 has NOT been authorized or started. Step 1 results above are historical and do not establish Step 2 runtime behavior.

## Build environment and result

Windows NT 10.0.22631.0 x64; ESP-IDF v5.5.4 at C:/Espressif/frameworks/esp-idf-v5.5.4, commit 735507283d5b2f9fb363a1901172dbd9e847945d. Tracked SDK diff remains empty; recursive submodules match pinned revisions. Unrelated pre-existing untracked SDK projects remain untouched. Tools: Python 3.11.2, GCC 14.2.0 (esp-14.2.0_20260121), CMake 3.30.2, Ninja 1.12.1, esptool v4.12.dev2.

The first attempt stalled in compiler-cache subprocesses and was interrupted. The retry disabled ccache. Git ownership checks initially prevented version discovery; resolved using process-local safe.directory entries for this project, the verified SDK and its subdirectories, without global Git or SDK changes. Final reconfiguration/build/size inspection exited 0. No main.c compiler warnings or errors were emitted. Shell activation reported unused older tools; none was removed. No implementation-design deviation was required.

From the firmware project directory, initialize the shell and build:

```powershell
$env:IDF_TOOLS_PATH = 'C:\Espressif'
$env:PATH = 'C:\Espressif\python_env\idf5.5_py3.11_env\Scripts;' + $env:PATH
. C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1
idf.py --no-ccache -DIDF_TARGET=esp32s3 reconfigure build size
```

When running under the sandbox account rather than the installation owner, the build additionally used process-local GIT_CONFIG_COUNT/KEY/VALUE settings for safe.directory: C:/Espressif/frameworks/esp-idf-v5.5.4, C:/Espressif/frameworks/esp-idf-v5.5.4/* and D:/proff/clients/Welmade/CoAP-UDP-Trial. This only establishes trust for those verified local repositories in the build process; no global configuration was written.

The local .private/wifi_credentials.h must contain the two approved string-literal macros. Its actual values and all generated firmware/ELF outputs remain ignored and untracked. CMake checks header presence before component registration. No package installation required.

Effective configuration: esp32s3, 16 MB flash, CONFIG_ESPTOOLPY_FLASHMODE_QIO=y, 40 MHz flash, 40 MHz crystal, PSRAM disabled, 4096-byte main task stack, UART 115200, single factory application partition (1 MiB), FreeRTOS tick rate 100 Hz. The shared Wi-Fi budget is therefore 3000 ticks. ESP-IDF's generated flash command/header uses dio for initial bootloading even with QIO selected; the SDK bootloader upgrades to quad mode. This behavior is explicit in components/esptool_py/Kconfig.projbuild and is not a configuration deviation.

Representative final build output:

```text
-- App "esp32_udp_client" version: c2b2b7a-dirty
Successfully created esp32s3 image.
Bootloader binary size 0x5700 bytes. 0x2900 bytes (32%) free.
esp32_udp_client.bin binary size 0xb90a0 bytes.
Smallest app partition is 0x100000 bytes. 0x46f60 bytes (28%) free.
Project build complete.
```

The app version describes the pre-commit working tree used for this build; it does not indicate a modified SDK. The source hash below identifies the actual built main.c. A later rebuild after commit may have different app metadata and image hashes.

## Build artifact observations (not runtime measurements)

| Item | Observed value |
| --- | --- |
| Application binary | 757920 bytes |
| Application partition | 1048576 bytes; 290656 bytes free |
| Bootloader binary | 22272 bytes |
| Size-tool total image | 757797 bytes before binary padding |
| DIRAM used | 110135 bytes: text 74579, data 19964, bss 15592 |
| Flash code | 539766 bytes |
| Flash data | 107072 bytes |
| IRAM section used | 16384 bytes in the size-tool reported 16384-byte region |
| RTC slow / fast | 32 / 24 bytes |

No runtime free-heap, minimum-heap, stack-headroom, Wi-Fi association, UDP timing or exchange-success measurements exist for this firmware yet. No architectural conclusion is drawn from the size report.

SHA-256 evidence for this local build:

| File relative to firmware project | SHA-256 |
| --- | --- |
| main/main.c | f5e306b74759f66a02463ba552afcfe570e09c9e7b45fd7017564486aa6bf7a5 |
| build/esp32_udp_client.bin | 2ce56259093cadec76946ad5d3d5226634e5a894e02618e9209956b1af2e3af5 |
| build/esp32_udp_client.elf | 4a39c87a5c3ba8d04677e403512f889975dfad4f63997ed2835828c0b245ded2 |
| build/bootloader/bootloader.bin | d161ddc1b6a6846cf9bd68d76d27cdd2cb1d70e8a5f48b769dfb6a56024ac76d |
| sdkconfig | 03e46ff2ab04834743ee18e0dd63223202d9a3672f5a1ae2ede63049e8e4a9d2 |

## G3 review status and limitations

Actual firmware has exactly app_main and wifi_event_handler. The callback handles STARTED/READY/FAILED bits; only app_main connects, once after STARTED. Tick subtraction is unsigned and elapsed is checked before remaining-budget subtraction. Attempts increment immediately before send. Exact four-byte PONG plus valid Wi-Fi bits is required for success. Stop on first failure; no retries or custom tasks. Cleanup follows the approved guarded sequence. Full signatures, dependencies, types and ownership are in FUNCTION_REGISTRY.md and VARIABLE_REGISTRY.md.

Existing cloud.js, README.md and SDK source were not modified. No board flashing, reset, serial monitoring or network testing was performed in G3. SDK default-interface creation includes SDK-owned assertion/error-check behavior internally; allocation-failure behavior of that constructor has not been fault-injected. Cleanup failures and all runtime paths remain unverified until approved G4 work. No claim is made that a successful build establishes runtime acceptance.

## Approved G4 plan — deferred, not executed as a G4 suite

The following suite requires a separate explicit G4 authorization. G3 build/packaging inspections above are not reported as G4 PASS results.

| ID | Purpose / input | Expected result | G4 actual / status |
| --- | --- | --- | --- |
| S2-T01 | SDK/toolchain inspection | Exact v5.5.4 tracked baseline, pinned submodules and tools recorded | Not executed as G4; deferred |
| S2-T02 | Build without private header | Configuration error without secret disclosure | Not executed; deferred |
| S2-T03 | Build with local credentials | Successful ESP32-S3 build; record warnings/effective configuration | G3 build evidence above; G4 suite deferred |
| S2-T04 | Credential/ignore review | No private values or generated images tracked | G3 packaging review only; G4 suite deferred |
| S2-T05 | Flash/boot on COM14 | Target starts with expected configuration; no whole-chip erase | Not executed; deferred |
| S2-T06 | Approved Wi-Fi available | IPv4 obtained within shared 30-second startup/connection budget | Not executed; deferred |
| S2-T07 | Existing UDP server | attempts=20, successes=20; exact PING/PONG; endpoint correlation | Not executed; deferred |
| S2-T08 | Server stopped | attempts=1, successes=0; timeout/socket error; no subsequent send | Not executed; deferred |
| S2-T09 | Temporary peer sends FAIL, PONG+LF or oversized data in separate reset runs | Reject each; attempts=1, successes=0; no retry | Not executed; deferred |
| S2-T10 | PONG sent from wrong UDP source port | Not accepted; bounded failure; attempts=1, successes=0 | Not executed; deferred |
| S2-T11 | AP unavailable before boot | Failure/deadline; zero UDP attempts; no application reconnect | Not executed; deferred |
| S2-T12 | AP disabled during run | Latch FAILED; report actual counts; stop after detected failure | Not executed; deferred |
| S2-T13 | Completion and manual reset | Record cleanup/heap/stack; reset starts independent run | Not executed; deferred |
| S2-T14 | Static partial-init cleanup review | Only acquired resources accessed; callback lifetime protected | Not executed as formal G4 review; deferred |

For G4, record each setup, expected result, actual result, PASS/FAIL or not executed, representative logs and observations. Temporary command-line responders only; no permanent test-client file or cloud.js modification authorized. Runtime credential values must be omitted from tracked logs.


# Phase 1, Step 2 — G4 hardware test attempt (2026-11-11)

G3 source review was accepted and G4 explicitly authorized by the user. Implementation commit e7de4f70d1feb93853fcd6b78f72b3e89d4dfe8d; main.c SHA-256 unchanged: f5e306b74759f66a02463ba552afcfe570e09c9e7b45fd7017564486aa6bf7a5. No source, SDK, eFuse or security configuration changes made during testing.

## Setup and evidence

Same Windows/ESP-IDF 5.5.4 toolchain as G3. Rechecked Python 3.11.2, GCC 14.2.0, clean tracked SDK, and all 27 recursively listed submodules at pinned revisions. Submodule status output SHA-256: 853607acddbe222c52bc9f90cac048c294c171e0807cd73c045d436477c5dc76. PC Wi-Fi IPv4 observed as 192.168.0.170. No server was listening initially; started unchanged cloud.js on 0.0.0.0:56830 for the test and stopped that test-owned process when hardware testing was blocked. No firewall changes made.

Temporarily renamed the ignored private header within .private/, ran idf.py --no-ccache reconfigure, and restored it in a finally block. Configuration failed with exit 2 and the intended missing-header message. This negative run also emitted a CMake developer syntax warning in generated build_properties.temp.cmake. With the header restored, reconfigure/build/size exited 0, without a main.c compiler warning. No credential values or generated/private artifacts were present in tracked files.

G4 application binary: 757920 bytes; SHA-256 35f77f2325b7dc54cd395f4f3abae5d9436a0be452ceacae5d38d8eb299ffb88. Rebuild metadata identifies the committed source; G3 hashes above remain historical evidence for the earlier pre-commit build. Flash/stack/PSRAM/console settings remained as approved.

Initial connection attempts with no_reset and automatic reset failed before writing. After user-assisted BOOT/EN entry to download mode, esptool wrote the generated bootloader at 0x0, application at 0x10000, and partition table at 0x8000. All three writes passed esptool hash verification. Erased ranges were limited to the flashed image sectors: 0x00000000-0x00005fff, 0x00010000-0x000c9fff, and 0x00008000-0x00008fff. No whole-chip erase or NVS erase performed.

The first serial capture was empty. Releasing the UART DTR/RTS lines and resetting exposed repeated ROM invalid-header messages. An initial capture command encountered a host cp1252 Unicode printing error; a subsequent UTF-8-safe capture preserved representative boot output locally in the ignored build directory. This host capture issue is separate from the observed ROM boot failure.

Representative actual output:

```text
Chip is ESP32-S3 (QFN56) (revision v0.2)
Features: WiFi, BLE, Embedded PSRAM 8MB (AP_3v3)
Crystal is 40MHz
Wrote 757920 bytes (475282 compressed) at 0x00010000
Hash of data verified.
ESP-ROM:esp32s3-20210327
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
invalid header: 0x71328111
rst:0x7 (TG0WDT_SYS_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
```

## Read-only root-cause investigation

Following another user-assisted download-mode entry, ran esptool --no-stub get_security_info without resetting afterward. It reported:

```text
Flags: 0x00000680
BLOCK_KEY0 - XTS_AES_128_KEY
Secure Boot: Disabled
Flash Encryption: Enabled
SPI Boot Crypt Count (SPI_BOOT_CRYPT_CNT): 0x1
Dcache in UART download mode: Disabled
Icache in UART download mode: Disabled
JTAG: Permanently Disabled
Staying in bootloader.
```

This is a security-state mismatch with the approved unencrypted firmware baseline. Espressif documents that an odd set-bit count in SPI_BOOT_CRYPT_CNT enables boot-time flash decryption, and that the bootloader is an encrypted image region: https://docs.espressif.com/projects/esp-idf/en/v5.5.4/esp32s3/security/flash-encryption.html . The successful plaintext writes do not establish boot compatibility with this provisioned device. No key bytes were read, no security state was changed, and no attempt was made to disable encryption or JTAG protection. The device was left in download mode.

## Test results

| Test | Purpose and input/setup | Expected result | Actual result / relevant evidence | Status |
| --- | --- | --- | --- | --- |
| S2-T01 | Inspect SDK/tools/submodules | Exact clean tracked v5.5.4 baseline and pinned dependencies | v5.5.4, commit 735507283d5b2f9fb363a1901172dbd9e847945d; no tracked SDK diff; 27 pinned submodules; Python/GCC verified | PASS |
| S2-T02 | Configure without private header | Fail clearly without disclosing credentials | Exit 2; Missing local .private/wifi_credentials.h; header restored afterward | PASS |
| S2-T03 | Configure/build with private header | Successful approved ESP32-S3 configuration/build | Exit 0; 757920-byte application, 28% app partition free; no main.c warning | PASS |
| S2-T04 | Review ignore rules and tracked content | Credentials/build outputs not tracked | Private header, sdkconfig and binary ignored; neither supplied credential value found in tracked file bytes | PASS |
| S2-T05 | Flash and boot COM14 | Verified image writes and application startup | Three writes hash-verified; ROM repeatedly reports invalid header; no ESP-IDF/application startup | FAIL (boot stage) |
| S2-T06 | Connect to approved Wi-Fi within 30 seconds | IPv4 acquired | Application never starts; Wi-Fi functionality not exercised | NOT EXECUTED |
| S2-T07 | Baseline UDP exchanges | attempts=20, successes=20 with endpoint correlation | No application execution; no ESP32 PING observed by server | NOT EXECUTED |
| S2-T08 | Server unavailable | One failed attempt, no further sends | Blocked by boot failure | NOT EXECUTED |
| S2-T09 | Temporary malformed-response peer | Reject FAIL, PONG+LF and oversized data in separate runs | Blocked by boot failure; no negative responder started | NOT EXECUTED |
| S2-T10 | Response from incorrect source port | Do not accept response | Blocked by boot failure | NOT EXECUTED |
| S2-T11 | AP unavailable before boot | Zero attempts; bounded failure | Blocked by boot failure; AP not changed | NOT EXECUTED |
| S2-T12 | AP disabled during exchanges | Latch failure and stop sends | Blocked by boot failure; AP not changed | NOT EXECUTED |
| S2-T13 | Completion/reset | Cleanup and resource logs; fresh run after reset | Application never starts; cannot observe cleanup or counters | NOT EXECUTED |
| S2-T14 | Static partial-initialization cleanup review | Guard acquired resources and callback lifetime | Acquisition flags/handles checked; exact unregister keys; group deletion gated on all registered callbacks being removed; dependent resources retained on shutdown failure; one connect owner and bounded tick arithmetic reviewed | PASS (static inspection only) |

Summary: 5 PASS, 1 FAIL, 8 NOT EXECUTED. No firmware UDP request/response exchange was observed. Application attempt/success counters, runtime heap and stack measurements are unavailable because app_main did not execute; they are not reported as zero-valued firmware measurements. No runtime fault-injection coverage is claimed by S2-T14, and SDK-internal assertion behavior remains untested.

## Correction/design status

Stop affected G4 execution at the observed boot failure. No firmware correction was implemented. Resuming requires either verification of an equivalent unencrypted target, or explicit user approval of revised deployment/security requirements for the existing provisioned target through the applicable design gate. No milestone acceptance, merge, tag or Step 3 work authorized by this test attempt.


# G4 recovery, retest and final available results (2026-11-11)

This section supplements, and does not replace, the original failed boot evidence. The user explicitly approved burning only SPI_BOOT_CRYPT_CNT from 0b001 to 0b011, followed by full-chip erase and reflash of the already approved firmware. The user subsequently corrected the private Wi-Fi SSID capitalization and explicitly directed that AP-unavailable and mid-run AP-disconnect tests remain unexecuted.

## Authorized recovery and configuration correction

Preflight readback: SPI_BOOT_CRYPT_CNT=0b001 and writable; Secure Boot disabled; DIS_DOWNLOAD_MANUAL_ENCRYPT=False. Generated firmware configuration had CONFIG_SECURE_FLASH_ENC_ENABLED unset. Burned only SPI_BOOT_CRYPT_CNT to 3. The tool reported:

```text
SPI_BOOT_CRYPT_CNT ... 0b001 -> 0b011
BURN BLOCK0 - OK
Checking efuses...
Successful
Flash Encryption: Disabled
SPI Boot Crypt Count (SPI_BOOT_CRYPT_CNT): 0x3
```

Full-chip erase completed in 3.1 seconds. Reflashed the generated bootloader, application and partition table; esptool verified all writes. No further eFuse field or protection setting was changed. Existing key purpose and disabled-JTAG state remain. This erase was specifically authorized after the earlier test attempt; the earlier no-whole-chip-erase record remains accurate for that earlier attempt.

First successful plaintext boot reached ESP-IDF v5.5.4 and app_main. That run could not find the initially configured network: reason=201 (NO_AP_FOUND), attempts=0, successes=0, then returned cleanly. User provided a case correction to the SSID. Changed only the ignored .private/wifi_credentials.h, rebuilt successfully and reflashed with all write hashes verified. Actual credentials are deliberately omitted here. No tracked firmware source or SDK change was needed. PC inspection confirmed it was connected to the corrected network on 2.4 GHz, channel 4.

Final tested binary: 757920 bytes, SHA-256 594f9b9ef0d0abc3d5ddc387f263380254bc862c25426ee3539a8334c1f8d762. Firmware source main.c SHA-256 remains f5e306b74759f66a02463ba552afcfe570e09c9e7b45fd7017564486aa6bf7a5, identical to G3. App version metadata for this rebuild: c10418f-dirty, reflecting documentation edits in progress. Final configuration still uses ESP-IDF 5.5.4, 16 MB QIO flash at 40 MHz, PSRAM disabled and 4096-byte main stack. Credential-bearing binaries and local sanitized captures remain ignored under build/; no permanent responder/test-client source file was created.

## Run observations

Each run was externally restarted using esptool's host-side HardReset over UART RTS, with DTR released. There were no application retries, reconnects or changes to the approved 20-attempt limit. Temporary malformed-response fixtures were inline NodeJS dgram commands; the existing cloud.js was unchanged.

| Run | Setup / input | Actual attempts / successes | Actual observation |
| --- | --- | --- | --- |
| Corrected-network baseline | Unchanged cloud.js at 192.168.0.170:56830 | 20 / 20 | Exact PONG throughout; IPv4 acquired at firmware tick-log time 2886 ms; final result at 22096 ms |
| Server absent, first setup attempt | No UDP listener | 0 / 0 | Wi-Fi association stopped first with reason=4; UDP condition not reached; retained as setup failure, not a successful timeout test |
| Server absent, independent run 2 | Still no UDP listener | 1 / 0 | Receive timeout errno=11 at 4196 ms after send at 2186 ms; returned from app_main; no second send |
| Malformed response: FAIL | Temporary peer responds with four nonmatching bytes | 1 / 0 | Rejected length-4 nonmatching response; one request observed by peer |
| Malformed response: PONG+LF | Temporary peer sends five bytes | 1 / 0 | Rejected length-5 response; one request observed by peer |
| Oversized response | Temporary peer sends 100 bytes | 1 / 0 | recv returned 64 bytes (buffer limit); response rejected; one request observed by peer |
| Wrong source port | Temporary peer sends exact PONG from port 53481 rather than 56830 | 1 / 0 | Receive timeout errno=11; wrong-port datagram not accepted; one request observed by fixture |
| Reset/completion run | Restart unchanged cloud.js and externally reset target after previous runs | 20 / 20 | Counters restarted; exact PONG throughout; final result at 21426 ms; returned from app_main |

Positive-run evidence: 40 valid responses from 40 requests across two independent 20-exchange runs, with zero positive-run timeouts. Controlled negative cases: five one-attempt runs, no accepted responses; two expected receive timeouts and three expected malformed-response rejections. The additional association setup failure had zero UDP attempts and is not hidden by a retry. No aggregate packet-loss percentage is inferred by combining intentional negative tests with the positive runs.

Original cloud logs matched ESP32 source 192.168.0.122:59185 for the first positive run and 192.168.0.122:60946 for the reset run, with 20 received PING lines and 20 corresponding sender-directed PONG lines in each. ESP32 getsockname logged wildcard 0.0.0.0 plus the matching ephemeral port; the acquired IPv4 event and cloud-side source logs establish the actual interface address. No change was made to address logging.

Representative actual output:

```text
I (347) spi_flash: detected chip: generic
I (349) spi_flash: flash io: qio
I (2886) udp_trial: IPv4 acquired: 192.168.0.122
I (2886) udp_trial: UDP local=0.0.0.0:59185 peer=192.168.0.170:56830
I (22096) udp_trial: Result: attempts=20 successes=20 status=PASS
I (22096) udp_trial: Internal heap: free=277576 minimum=275460 bytes; main stack minimum free=1748 bytes
Received from 192.168.0.122:59185 bytes=4 payload="PING"
Sent PONG to 192.168.0.122:59185
E (4196) udp_trial: Exchange 1 receive timeout: errno=11
I (4196) udp_trial: Result: attempts=1 successes=0 status=FAIL
E (2426) udp_trial: Exchange 1 unexpected response: bytes=64 (expected exact PONG)
I (21426) udp_trial: Result: attempts=20 successes=20 status=PASS
I (21426) udp_trial: Internal heap: free=277576 minimum=274008 bytes; main stack minimum free=1748 bytes
I (21506) main_task: Returned from app_main()
```

### Runtime resource observations

| Measurement | Baseline | Reset/completion run |
| --- | --- | --- |
| Internal 8-bit heap free before Wi-Fi | 329724 bytes | 329724 bytes |
| Internal 8-bit heap free after IPv4 | 278212 bytes | 278200 bytes |
| Internal heap free at result, before cleanup | 277576 bytes | 277576 bytes |
| Minimum internal heap observed by result | 275460 bytes | 274008 bytes |
| Main stack minimum free by result | 1748 bytes | 1748 bytes |

These are two observed runs on this board/network, not guaranteed worst-case resource bounds. The program logs metrics before teardown, so these are not post-cleanup heap measurements. Both positive runs reached the SDK's Returned from app_main line without cleanup warnings or assertion/panic. An error-level disconnect reason=8 line appears during intentional esp_wifi_stop after the PASS summary; it is a cleanup-generated disconnect, not a failed exchange. Failure-path cleanup runs also returned without cleanup warnings. No runtime allocation/unregister failure injection is claimed.

## Final G4 status for review

| Test | Purpose / input and expected result | Actual result / evidence | Status |
| --- | --- | --- | --- |
| S2-T01 | Verify exact SDK and pinned tools/submodules | Prior G4 inspection passed; SDK/source baseline unchanged during recovery | PASS |
| S2-T02 | Missing private header must fail without exposing secrets | Prior G4 configure exit 2 with intended message; header restored | PASS |
| S2-T03 | Build with private header and approved settings | Corrected private configuration rebuilt successfully; 757920-byte image; no main.c compiler warnings | PASS |
| S2-T04 | Credentials/generated outputs excluded from tracked content | Private/header/build ignore rules retained; credential values omitted from tracked evidence | PASS |
| S2-T05 | Verified flashing and successful application startup | Authorized recovery resolved original boot failure; all writes verified; ESP-IDF/app_main booted | PASS on retest; original FAIL retained |
| S2-T06 | IPv4 within shared 30-second deadline | Corrected configuration acquired IPv4 at 2886/2206 ms in positive runs | PASS on retest; earlier network/setup failures retained |
| S2-T07 | At most 20 attempts; positive baseline must achieve 20/20 with endpoint correlation | First positive run 20/20; cloud source/destination ports matched | PASS |
| S2-T08 | Server absent: one failed attempt, no subsequent send | Independent qualifying run: 1/0, errno=11 timeout, one send and clean return; earlier association setup failure recorded separately | PASS on qualifying run |
| S2-T09 | Reject FAIL, PONG+LF and oversized reply | Three separate runs each 1/0, one request, immediate rejection and clean return | PASS |
| S2-T10 | Ignore exact PONG from wrong source port | Source port 53481; 1/0, timeout, no second request and clean return | PASS |
| S2-T11 | AP unavailable before boot | User explicitly directed this AP-control test to remain unexecuted | NOT EXECUTED |
| S2-T12 | Disable AP during exchanges | User explicitly directed this AP-control test to remain unexecuted | NOT EXECUTED |
| S2-T13 | Completion, resource logs and externally initiated reset | Second positive run restarted counters and achieved 20/20; resource logs and clean main return observed | PASS |
| S2-T14 | Static ownership/callback-lifetime review | Earlier static review passed; firmware source unchanged; no runtime fault injection claimed | PASS (static inspection) |

Final available suite: 12 PASS, 2 NOT EXECUTED by user direction. Historical ROM boot failure, initial SSID mismatch and separate association setup failure remain part of the evidence. No claim of all-tests-executed or Phase 1 acceptance. No firmware design deviation or custom function/task/dependency change. The user-approved eFuse/erase deployment exception and private credential correction are documented explicitly.

The final test-owned cloud process and temporary responders were stopped, serial handles closed, and the board left running the approved firmware after its completed run (Wi-Fi stopped by firmware cleanup; no ongoing UDP traffic). No firewall changes, merge, tag or Step 3 work performed. Independent result review and user acceptance remain outstanding.


# Step 3 reproducibility archive — existing artifacts only (2026-11-11)

The source hashes are over canonical committed Git bytes at b13620f, avoiding checkout line-ending ambiguity. Local artifact hashes were compared to existing G4 evidence where available. No artifact was regenerated.

| Source / retained artifact | SHA-256 |
| --- | --- |
| phase1-udp-baseline/src/cloud.js | a388f8d4b3d86b3a50686f74be58527cbd475079f4aa665466079363bffab913 |
| phase1-udp-baseline/src/esp32-udp-client/main/main.c | f5e306b74759f66a02463ba552afcfe570e09c9e7b45fd7017564486aa6bf7a5 |
| phase1-udp-baseline/src/esp32-udp-client/CMakeLists.txt | 72fb41d7cafd593af5be84089925b3fc774f954bc140325360acc9afa088319f |
| phase1-udp-baseline/src/esp32-udp-client/main/CMakeLists.txt | 899709975911d99dbb2b1e4041d338a51f7ca34fc653b201a2f486079e4351ce |
| phase1-udp-baseline/src/esp32-udp-client/sdkconfig.defaults | 1a254573624b05004a0a3a5ee7793945dde0a71cc477b78c067d73fca58df707 |
| phase1-udp-baseline/src/esp32-udp-client/build/esp32_udp_client.bin (local) | 594f9b9ef0d0abc3d5ddc387f263380254bc862c25426ee3539a8334c1f8d762 |
| phase1-udp-baseline/src/esp32-udp-client/build/esp32_udp_client.elf (local) | 3f1a646aade782be38d87432081eeaf14480a33da3d2e6eda39cac9e03527e66 |
| phase1-udp-baseline/src/esp32-udp-client/build/bootloader/bootloader.bin (local) | d161ddc1b6a6846cf9bd68d76d27cdd2cb1d70e8a5f48b769dfb6a56024ac76d |
| phase1-udp-baseline/src/esp32-udp-client/sdkconfig (local) | 03e46ff2ab04834743ee18e0dd63223202d9a3672f5a1ae2ede63049e8e4a9d2 |

## Effective non-secret sdkconfig snapshot

The complete existing sdkconfig is archived here because the generated file itself is deliberately ignored. Original file SHA-256: 03e46ff2ab04834743ee18e0dd63223202d9a3672f5a1ae2ede63049e8e4a9d2. Markdown may normalize line endings when copied; retain the settings and compare normalized content rather than assuming identical extracted-file bytes. No Wi-Fi credential values are present in this configuration; they were supplied separately through the ignored header. Reusing this snapshot, source and toolchain does not authorize building/flashing/testing without the applicable approval.

```text
#
# Automatically generated file. DO NOT EDIT.
# Espressif IoT Development Framework (ESP-IDF) 5.5.4 Project Configuration
#
CONFIG_SOC_ADC_SUPPORTED=y
CONFIG_SOC_UART_SUPPORTED=y
CONFIG_SOC_PCNT_SUPPORTED=y
CONFIG_SOC_PHY_SUPPORTED=y
CONFIG_SOC_WIFI_SUPPORTED=y
CONFIG_SOC_TWAI_SUPPORTED=y
CONFIG_SOC_GDMA_SUPPORTED=y
CONFIG_SOC_UHCI_SUPPORTED=y
CONFIG_SOC_AHB_GDMA_SUPPORTED=y
CONFIG_SOC_GPTIMER_SUPPORTED=y
CONFIG_SOC_LCDCAM_SUPPORTED=y
CONFIG_SOC_LCDCAM_CAM_SUPPORTED=y
CONFIG_SOC_LCDCAM_I80_LCD_SUPPORTED=y
CONFIG_SOC_LCDCAM_RGB_LCD_SUPPORTED=y
CONFIG_SOC_MCPWM_SUPPORTED=y
CONFIG_SOC_DEDICATED_GPIO_SUPPORTED=y
CONFIG_SOC_CACHE_SUPPORT_WRAP=y
CONFIG_SOC_ULP_SUPPORTED=y
CONFIG_SOC_ULP_FSM_SUPPORTED=y
CONFIG_SOC_RISCV_COPROC_SUPPORTED=y
CONFIG_SOC_BT_SUPPORTED=y
CONFIG_SOC_USB_OTG_SUPPORTED=y
CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED=y
CONFIG_SOC_CCOMP_TIMER_SUPPORTED=y
CONFIG_SOC_ASYNC_MEMCPY_SUPPORTED=y
CONFIG_SOC_SUPPORTS_SECURE_DL_MODE=y
CONFIG_SOC_EFUSE_KEY_PURPOSE_FIELD=y
CONFIG_SOC_EFUSE_SUPPORTED=y
CONFIG_SOC_SDMMC_HOST_SUPPORTED=y
CONFIG_SOC_RTC_FAST_MEM_SUPPORTED=y
CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED=y
CONFIG_SOC_RTC_MEM_SUPPORTED=y
CONFIG_SOC_PSRAM_DMA_CAPABLE=y
CONFIG_SOC_XT_WDT_SUPPORTED=y
CONFIG_SOC_I2S_SUPPORTED=y
CONFIG_SOC_RMT_SUPPORTED=y
CONFIG_SOC_SDM_SUPPORTED=y
CONFIG_SOC_GPSPI_SUPPORTED=y
CONFIG_SOC_LEDC_SUPPORTED=y
CONFIG_SOC_I2C_SUPPORTED=y
CONFIG_SOC_SYSTIMER_SUPPORTED=y
CONFIG_SOC_SUPPORT_COEXISTENCE=y
CONFIG_SOC_TEMP_SENSOR_SUPPORTED=y
CONFIG_SOC_AES_SUPPORTED=y
CONFIG_SOC_MPI_SUPPORTED=y
CONFIG_SOC_SHA_SUPPORTED=y
CONFIG_SOC_HMAC_SUPPORTED=y
CONFIG_SOC_DIG_SIGN_SUPPORTED=y
CONFIG_SOC_FLASH_ENC_SUPPORTED=y
CONFIG_SOC_SECURE_BOOT_SUPPORTED=y
CONFIG_SOC_MEMPROT_SUPPORTED=y
CONFIG_SOC_TOUCH_SENSOR_SUPPORTED=y
CONFIG_SOC_BOD_SUPPORTED=y
CONFIG_SOC_CLK_TREE_SUPPORTED=y
CONFIG_SOC_MPU_SUPPORTED=y
CONFIG_SOC_WDT_SUPPORTED=y
CONFIG_SOC_SPI_FLASH_SUPPORTED=y
CONFIG_SOC_RNG_SUPPORTED=y
CONFIG_SOC_LIGHT_SLEEP_SUPPORTED=y
CONFIG_SOC_DEEP_SLEEP_SUPPORTED=y
CONFIG_SOC_LP_PERIPH_SHARE_INTERRUPT=y
CONFIG_SOC_PM_SUPPORTED=y
CONFIG_SOC_SIMD_INSTRUCTION_SUPPORTED=y
CONFIG_SOC_XTAL_SUPPORT_40M=y
CONFIG_SOC_APPCPU_HAS_CLOCK_GATING_BUG=y
CONFIG_SOC_ADC_RTC_CTRL_SUPPORTED=y
CONFIG_SOC_ADC_DIG_CTRL_SUPPORTED=y
CONFIG_SOC_ADC_ARBITER_SUPPORTED=y
CONFIG_SOC_ADC_DIG_IIR_FILTER_SUPPORTED=y
CONFIG_SOC_ADC_MONITOR_SUPPORTED=y
CONFIG_SOC_ADC_DMA_SUPPORTED=y
CONFIG_SOC_ADC_PERIPH_NUM=2
CONFIG_SOC_ADC_MAX_CHANNEL_NUM=10
CONFIG_SOC_ADC_ATTEN_NUM=4
CONFIG_SOC_ADC_DIGI_CONTROLLER_NUM=2
CONFIG_SOC_ADC_PATT_LEN_MAX=24
CONFIG_SOC_ADC_DIGI_MIN_BITWIDTH=12
CONFIG_SOC_ADC_DIGI_MAX_BITWIDTH=12
CONFIG_SOC_ADC_DIGI_RESULT_BYTES=4
CONFIG_SOC_ADC_DIGI_DATA_BYTES_PER_CONV=4
CONFIG_SOC_ADC_DIGI_IIR_FILTER_NUM=2
CONFIG_SOC_ADC_DIGI_MONITOR_NUM=2
CONFIG_SOC_ADC_SAMPLE_FREQ_THRES_HIGH=83333
CONFIG_SOC_ADC_SAMPLE_FREQ_THRES_LOW=611
CONFIG_SOC_ADC_RTC_MIN_BITWIDTH=12
CONFIG_SOC_ADC_RTC_MAX_BITWIDTH=12
CONFIG_SOC_ADC_CALIBRATION_V1_SUPPORTED=y
CONFIG_SOC_ADC_SELF_HW_CALI_SUPPORTED=y
CONFIG_SOC_ADC_SHARED_POWER=y
CONFIG_SOC_APB_BACKUP_DMA=y
CONFIG_SOC_BROWNOUT_RESET_SUPPORTED=y
CONFIG_SOC_CACHE_WRITEBACK_SUPPORTED=y
CONFIG_SOC_CACHE_FREEZE_SUPPORTED=y
CONFIG_SOC_CACHE_ACS_INVALID_STATE_ON_PANIC=y
CONFIG_SOC_CPU_CORES_NUM=2
CONFIG_SOC_CPU_INTR_NUM=32
CONFIG_SOC_CPU_HAS_FPU=y
CONFIG_SOC_HP_CPU_HAS_MULTIPLE_CORES=y
CONFIG_SOC_CPU_BREAKPOINTS_NUM=2
CONFIG_SOC_CPU_WATCHPOINTS_NUM=2
CONFIG_SOC_CPU_WATCHPOINT_MAX_REGION_SIZE=0x40
CONFIG_SOC_SIMD_PREFERRED_DATA_ALIGNMENT=16
CONFIG_SOC_DS_SIGNATURE_MAX_BIT_LEN=4096
CONFIG_SOC_DS_KEY_PARAM_MD_IV_LENGTH=16
CONFIG_SOC_DS_KEY_CHECK_MAX_WAIT_US=1100
CONFIG_SOC_AHB_GDMA_VERSION=1
CONFIG_SOC_GDMA_NUM_GROUPS_MAX=1
CONFIG_SOC_GDMA_PAIRS_PER_GROUP=5
CONFIG_SOC_GDMA_PAIRS_PER_GROUP_MAX=5
CONFIG_SOC_AHB_GDMA_SUPPORT_PSRAM=y
CONFIG_SOC_GPIO_PORT=1
CONFIG_SOC_GPIO_PIN_COUNT=49
CONFIG_SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER=y
CONFIG_SOC_GPIO_FILTER_CLK_SUPPORT_APB=y
CONFIG_SOC_GPIO_SUPPORT_RTC_INDEPENDENT=y
CONFIG_SOC_GPIO_SUPPORT_FORCE_HOLD=y
CONFIG_SOC_GPIO_VALID_GPIO_MASK=0x1FFFFFFFFFFFF
CONFIG_SOC_GPIO_IN_RANGE_MAX=48
CONFIG_SOC_GPIO_OUT_RANGE_MAX=48
CONFIG_SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK=0x0001FFFFFC000000
CONFIG_SOC_GPIO_CLOCKOUT_BY_IO_MUX=y
CONFIG_SOC_GPIO_CLOCKOUT_CHANNEL_NUM=3
CONFIG_SOC_DEDIC_GPIO_OUT_CHANNELS_NUM=8
CONFIG_SOC_DEDIC_GPIO_IN_CHANNELS_NUM=8
CONFIG_SOC_DEDIC_GPIO_OUT_AUTO_ENABLE=y
CONFIG_SOC_I2C_NUM=2
CONFIG_SOC_HP_I2C_NUM=2
CONFIG_SOC_I2C_FIFO_LEN=32
CONFIG_SOC_I2C_CMD_REG_NUM=8
CONFIG_SOC_I2C_SUPPORT_SLAVE=y
CONFIG_SOC_I2C_SUPPORT_HW_CLR_BUS=y
CONFIG_SOC_I2C_SUPPORT_XTAL=y
CONFIG_SOC_I2C_SUPPORT_RTC=y
CONFIG_SOC_I2C_SUPPORT_10BIT_ADDR=y
CONFIG_SOC_I2C_SLAVE_SUPPORT_BROADCAST=y
CONFIG_SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS=y
CONFIG_SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE=y
CONFIG_SOC_I2S_NUM=2
CONFIG_SOC_I2S_HW_VERSION_2=y
CONFIG_SOC_I2S_SUPPORTS_XTAL=y
CONFIG_SOC_I2S_SUPPORTS_PLL_F160M=y
CONFIG_SOC_I2S_SUPPORTS_PCM=y
CONFIG_SOC_I2S_SUPPORTS_PDM=y
CONFIG_SOC_I2S_SUPPORTS_PDM_TX=y
CONFIG_SOC_I2S_SUPPORTS_PCM2PDM=y
CONFIG_SOC_I2S_SUPPORTS_PDM_RX=y
CONFIG_SOC_I2S_SUPPORTS_PDM2PCM=y
CONFIG_SOC_I2S_PDM_MAX_TX_LINES=2
CONFIG_SOC_I2S_PDM_MAX_RX_LINES=4
CONFIG_SOC_I2S_SUPPORTS_TDM=y
CONFIG_SOC_LEDC_SUPPORT_APB_CLOCK=y
CONFIG_SOC_LEDC_SUPPORT_XTAL_CLOCK=y
CONFIG_SOC_LEDC_TIMER_NUM=4
CONFIG_SOC_LEDC_CHANNEL_NUM=8
CONFIG_SOC_LEDC_TIMER_BIT_WIDTH=14
CONFIG_SOC_LEDC_SUPPORT_FADE_STOP=y
CONFIG_SOC_MCPWM_GROUPS=2
CONFIG_SOC_MCPWM_TIMERS_PER_GROUP=3
CONFIG_SOC_MCPWM_OPERATORS_PER_GROUP=3
CONFIG_SOC_MCPWM_COMPARATORS_PER_OPERATOR=2
CONFIG_SOC_MCPWM_GENERATORS_PER_OPERATOR=2
CONFIG_SOC_MCPWM_TRIGGERS_PER_OPERATOR=2
CONFIG_SOC_MCPWM_GPIO_FAULTS_PER_GROUP=3
CONFIG_SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP=y
CONFIG_SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER=3
CONFIG_SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP=3
CONFIG_SOC_MCPWM_SWSYNC_CAN_PROPAGATE=y
CONFIG_SOC_MMU_LINEAR_ADDRESS_REGION_NUM=1
CONFIG_SOC_MMU_PERIPH_NUM=1
CONFIG_SOC_MPU_MIN_REGION_SIZE=0x20000000
CONFIG_SOC_MPU_REGIONS_MAX_NUM=8
CONFIG_SOC_PCNT_GROUPS=1
CONFIG_SOC_PCNT_UNITS_PER_GROUP=4
CONFIG_SOC_PCNT_CHANNELS_PER_UNIT=2
CONFIG_SOC_PCNT_THRES_POINT_PER_UNIT=2
CONFIG_SOC_RMT_GROUPS=1
CONFIG_SOC_RMT_TX_CANDIDATES_PER_GROUP=4
CONFIG_SOC_RMT_RX_CANDIDATES_PER_GROUP=4
CONFIG_SOC_RMT_CHANNELS_PER_GROUP=8
CONFIG_SOC_RMT_MEM_WORDS_PER_CHANNEL=48
CONFIG_SOC_RMT_SUPPORT_RX_PINGPONG=y
CONFIG_SOC_RMT_SUPPORT_RX_DEMODULATION=y
CONFIG_SOC_RMT_SUPPORT_ASYNC_STOP=y
CONFIG_SOC_RMT_SUPPORT_TX_LOOP_COUNT=y
CONFIG_SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP=y
CONFIG_SOC_RMT_SUPPORT_TX_SYNCHRO=y
CONFIG_SOC_RMT_SUPPORT_TX_CARRIER_DATA_ONLY=y
CONFIG_SOC_RMT_SUPPORT_XTAL=y
CONFIG_SOC_RMT_SUPPORT_RC_FAST=y
CONFIG_SOC_RMT_SUPPORT_APB=y
CONFIG_SOC_RMT_SUPPORT_DMA=y
CONFIG_SOC_LCD_I80_SUPPORTED=y
CONFIG_SOC_LCD_RGB_SUPPORTED=y
CONFIG_SOC_LCD_I80_BUSES=1
CONFIG_SOC_LCD_RGB_PANELS=1
CONFIG_SOC_LCD_I80_BUS_WIDTH=16
CONFIG_SOC_LCD_RGB_DATA_WIDTH=16
CONFIG_SOC_LCD_SUPPORT_RGB_YUV_CONV=y
CONFIG_SOC_LCDCAM_I80_NUM_BUSES=1
CONFIG_SOC_LCDCAM_I80_BUS_WIDTH=16
CONFIG_SOC_LCDCAM_RGB_NUM_PANELS=1
CONFIG_SOC_LCDCAM_RGB_DATA_WIDTH=16
CONFIG_SOC_RTC_CNTL_CPU_PD_DMA_BUS_WIDTH=128
CONFIG_SOC_RTC_CNTL_CPU_PD_REG_FILE_NUM=549
CONFIG_SOC_RTC_CNTL_TAGMEM_PD_DMA_BUS_WIDTH=128
CONFIG_SOC_RTC_CNTL_NEEDS_ATOMIC_ACCESS=y
CONFIG_SOC_RTCIO_PIN_COUNT=22
CONFIG_SOC_RTCIO_INPUT_OUTPUT_SUPPORTED=y
CONFIG_SOC_RTCIO_HOLD_SUPPORTED=y
CONFIG_SOC_RTCIO_WAKE_SUPPORTED=y
CONFIG_SOC_LP_IO_CLOCK_IS_INDEPENDENT=y
CONFIG_SOC_SDM_GROUPS=1
CONFIG_SOC_SDM_CHANNELS_PER_GROUP=8
CONFIG_SOC_SDM_CLK_SUPPORT_APB=y
CONFIG_SOC_SPI_PERIPH_NUM=3
CONFIG_SOC_SPI_MAX_CS_NUM=6
CONFIG_SOC_SPI_MAXIMUM_BUFFER_SIZE=64
CONFIG_SOC_SPI_SUPPORT_DDRCLK=y
CONFIG_SOC_SPI_SLAVE_SUPPORT_SEG_TRANS=y
CONFIG_SOC_SPI_SUPPORT_CD_SIG=y
CONFIG_SOC_SPI_SUPPORT_CONTINUOUS_TRANS=y
CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2=y
CONFIG_SOC_SPI_SUPPORT_CLK_APB=y
CONFIG_SOC_SPI_SUPPORT_CLK_XTAL=y
CONFIG_SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUT=y
CONFIG_SOC_MEMSPI_IS_INDEPENDENT=y
CONFIG_SOC_SPI_MAX_PRE_DIVIDER=16
CONFIG_SOC_SPI_SUPPORT_OCT=y
CONFIG_SOC_SPI_SCT_SUPPORTED=y
CONFIG_SOC_SPI_SCT_REG_NUM=14
CONFIG_SOC_SPI_SCT_BUFFER_NUM_MAX=y
CONFIG_SOC_SPI_SCT_CONF_BITLEN_MAX=0x3FFFA
CONFIG_SOC_SPIRAM_SUPPORTED=y
CONFIG_SOC_SPIRAM_XIP_SUPPORTED=y
CONFIG_SOC_SYSTIMER_COUNTER_NUM=2
CONFIG_SOC_SYSTIMER_ALARM_NUM=3
CONFIG_SOC_SYSTIMER_BIT_WIDTH_LO=32
CONFIG_SOC_SYSTIMER_BIT_WIDTH_HI=20
CONFIG_SOC_SYSTIMER_FIXED_DIVIDER=y
CONFIG_SOC_SYSTIMER_INT_LEVEL=y
CONFIG_SOC_SYSTIMER_ALARM_MISS_COMPENSATE=y
CONFIG_SOC_TIMER_GROUPS=2
CONFIG_SOC_TIMER_GROUP_TIMERS_PER_GROUP=2
CONFIG_SOC_TIMER_GROUP_COUNTER_BIT_WIDTH=54
CONFIG_SOC_TIMER_GROUP_SUPPORT_XTAL=y
CONFIG_SOC_TIMER_GROUP_SUPPORT_APB=y
CONFIG_SOC_TIMER_GROUP_TOTAL_TIMERS=4
CONFIG_SOC_LP_TIMER_BIT_WIDTH_LO=32
CONFIG_SOC_LP_TIMER_BIT_WIDTH_HI=16
CONFIG_SOC_TOUCH_SENSOR_VERSION=2
CONFIG_SOC_TOUCH_SENSOR_NUM=15
CONFIG_SOC_TOUCH_MIN_CHAN_ID=1
CONFIG_SOC_TOUCH_MAX_CHAN_ID=14
CONFIG_SOC_TOUCH_SUPPORT_BENCHMARK=y
CONFIG_SOC_TOUCH_SUPPORT_SLEEP_WAKEUP=y
CONFIG_SOC_TOUCH_SUPPORT_WATERPROOF=y
CONFIG_SOC_TOUCH_SUPPORT_PROX_SENSING=y
CONFIG_SOC_TOUCH_SUPPORT_DENOISE_CHAN=y
CONFIG_SOC_TOUCH_PROXIMITY_CHANNEL_NUM=3
CONFIG_SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED=y
CONFIG_SOC_TOUCH_SAMPLE_CFG_NUM=1
CONFIG_SOC_TWAI_CONTROLLER_NUM=1
CONFIG_SOC_TWAI_MASK_FILTER_NUM=1
CONFIG_SOC_TWAI_CLK_SUPPORT_APB=y
CONFIG_SOC_TWAI_BRP_MIN=2
CONFIG_SOC_TWAI_BRP_MAX=16384
CONFIG_SOC_TWAI_SUPPORTS_RX_STATUS=y
CONFIG_SOC_UART_NUM=3
CONFIG_SOC_UART_HP_NUM=3
CONFIG_SOC_UART_FIFO_LEN=128
CONFIG_SOC_UART_BITRATE_MAX=5000000
CONFIG_SOC_UART_SUPPORT_FSM_TX_WAIT_SEND=y
CONFIG_SOC_UART_SUPPORT_WAKEUP_INT=y
CONFIG_SOC_UART_SUPPORT_APB_CLK=y
CONFIG_SOC_UART_SUPPORT_RTC_CLK=y
CONFIG_SOC_UART_SUPPORT_XTAL_CLK=y
CONFIG_SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE=y
CONFIG_SOC_UHCI_NUM=1
CONFIG_SOC_USB_OTG_PERIPH_NUM=1
CONFIG_SOC_SHA_DMA_MAX_BUFFER_SIZE=3968
CONFIG_SOC_SHA_SUPPORT_DMA=y
CONFIG_SOC_SHA_SUPPORT_RESUME=y
CONFIG_SOC_SHA_GDMA=y
CONFIG_SOC_SHA_SUPPORT_SHA1=y
CONFIG_SOC_SHA_SUPPORT_SHA224=y
CONFIG_SOC_SHA_SUPPORT_SHA256=y
CONFIG_SOC_SHA_SUPPORT_SHA384=y
CONFIG_SOC_SHA_SUPPORT_SHA512=y
CONFIG_SOC_SHA_SUPPORT_SHA512_224=y
CONFIG_SOC_SHA_SUPPORT_SHA512_256=y
CONFIG_SOC_SHA_SUPPORT_SHA512_T=y
CONFIG_SOC_MPI_MEM_BLOCKS_NUM=4
CONFIG_SOC_MPI_OPERATIONS_NUM=3
CONFIG_SOC_RSA_MAX_BIT_LEN=4096
CONFIG_SOC_AES_SUPPORT_DMA=y
CONFIG_SOC_AES_GDMA=y
CONFIG_SOC_AES_SUPPORT_AES_128=y
CONFIG_SOC_AES_SUPPORT_AES_256=y
CONFIG_SOC_PM_SUPPORT_EXT0_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_EXT1_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_EXT_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_WIFI_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_BT_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP=y
CONFIG_SOC_PM_SUPPORT_CPU_PD=y
CONFIG_SOC_PM_SUPPORT_TAGMEM_PD=y
CONFIG_SOC_PM_SUPPORT_RTC_PERIPH_PD=y
CONFIG_SOC_PM_SUPPORT_RC_FAST_PD=y
CONFIG_SOC_PM_SUPPORT_VDDSDIO_PD=y
CONFIG_SOC_PM_SUPPORT_MAC_BB_PD=y
CONFIG_SOC_PM_SUPPORT_MODEM_PD=y
CONFIG_SOC_CONFIGURABLE_VDDSDIO_SUPPORTED=y
CONFIG_SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY=y
CONFIG_SOC_PM_CPU_RETENTION_BY_RTCCNTL=y
CONFIG_SOC_PM_MODEM_RETENTION_BY_BACKUPDMA=y
CONFIG_SOC_PM_MODEM_PD_BY_SW=y
CONFIG_SOC_CLK_RC_FAST_D256_SUPPORTED=y
CONFIG_SOC_RTC_SLOW_CLK_SUPPORT_RC_FAST_D256=y
CONFIG_SOC_CLK_RC_FAST_SUPPORT_CALIBRATION=y
CONFIG_SOC_CLK_XTAL32K_SUPPORTED=y
CONFIG_SOC_CLK_LP_FAST_SUPPORT_XTAL_D2=y
CONFIG_SOC_EFUSE_DIS_DOWNLOAD_ICACHE=y
CONFIG_SOC_EFUSE_DIS_DOWNLOAD_DCACHE=y
CONFIG_SOC_EFUSE_HARD_DIS_JTAG=y
CONFIG_SOC_EFUSE_DIS_USB_JTAG=y
CONFIG_SOC_EFUSE_SOFT_DIS_JTAG=y
CONFIG_SOC_EFUSE_DIS_DIRECT_BOOT=y
CONFIG_SOC_EFUSE_DIS_ICACHE=y
CONFIG_SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK=y
CONFIG_SOC_SECURE_BOOT_V2_RSA=y
CONFIG_SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS=3
CONFIG_SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS=y
CONFIG_SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY=y
CONFIG_SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX=64
CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES=y
CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS=y
CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_128=y
CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_256=y
CONFIG_SOC_MEMPROT_CPU_PREFETCH_PAD_SIZE=16
CONFIG_SOC_MEMPROT_MEM_ALIGN_SIZE=256
CONFIG_SOC_PHY_DIG_REGS_MEM_SIZE=21
CONFIG_SOC_MAC_BB_PD_MEM_SIZE=192
CONFIG_SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH=12
CONFIG_SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE=y
CONFIG_SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND=y
CONFIG_SOC_SPI_MEM_SUPPORT_AUTO_RESUME=y
CONFIG_SOC_SPI_MEM_SUPPORT_SW_SUSPEND=y
CONFIG_SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE=y
CONFIG_SOC_SPI_MEM_SUPPORT_TIMING_TUNING=y
CONFIG_SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE=y
CONFIG_SOC_SPI_MEM_SUPPORT_WRAP=y
CONFIG_SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY=y
CONFIG_SOC_MEMSPI_CORE_CLK_SHARED_WITH_PSRAM=y
CONFIG_SOC_SPI_MEM_SUPPORT_CACHE_32BIT_ADDR_MAP=y
CONFIG_SOC_SPI_MEM_FLASH_SUPPORT_HPM=y
CONFIG_SOC_COEX_HW_PTI=y
CONFIG_SOC_EXTERNAL_COEX_LEADER_TX_LINE=y
CONFIG_SOC_SDMMC_USE_GPIO_MATRIX=y
CONFIG_SOC_SDMMC_NUM_SLOTS=2
CONFIG_SOC_SDMMC_SUPPORT_XTAL_CLOCK=y
CONFIG_SOC_SDMMC_DELAY_PHASE_NUM=4
CONFIG_SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC=y
CONFIG_SOC_WIFI_HW_TSF=y
CONFIG_SOC_WIFI_FTM_SUPPORT=y
CONFIG_SOC_WIFI_GCMP_SUPPORT=y
CONFIG_SOC_WIFI_WAPI_SUPPORT=y
CONFIG_SOC_WIFI_TXOP_SUPPORT=y
CONFIG_SOC_WIFI_CSI_SUPPORT=y
CONFIG_SOC_WIFI_MESH_SUPPORT=y
CONFIG_SOC_WIFI_SUPPORT_VARIABLE_BEACON_WINDOW=y
CONFIG_SOC_WIFI_PHY_NEEDS_USB_WORKAROUND=y
CONFIG_SOC_BLE_SUPPORTED=y
CONFIG_SOC_BLE_MESH_SUPPORTED=y
CONFIG_SOC_BLE_50_SUPPORTED=y
CONFIG_SOC_BLE_DEVICE_PRIVACY_SUPPORTED=y
CONFIG_SOC_BLUFI_SUPPORTED=y
CONFIG_SOC_ULP_HAS_ADC=y
CONFIG_SOC_PHY_COMBO_MODULE=y
CONFIG_SOC_LCDCAM_CAM_SUPPORT_RGB_YUV_CONV=y
CONFIG_SOC_LCDCAM_CAM_PERIPH_NUM=1
CONFIG_SOC_LCDCAM_CAM_DATA_WIDTH_MAX=16
CONFIG_IDF_CMAKE=y
CONFIG_IDF_TOOLCHAIN="gcc"
CONFIG_IDF_TOOLCHAIN_GCC=y
CONFIG_IDF_TARGET_ARCH_XTENSA=y
CONFIG_IDF_TARGET_ARCH="xtensa"
CONFIG_IDF_TARGET="esp32s3"
CONFIG_IDF_INIT_VERSION="5.5.4"
CONFIG_IDF_TARGET_ESP32S3=y
CONFIG_IDF_FIRMWARE_CHIP_ID=0x0009

#
# Build type
#
CONFIG_APP_BUILD_TYPE_APP_2NDBOOT=y
# CONFIG_APP_BUILD_TYPE_RAM is not set
CONFIG_APP_BUILD_GENERATE_BINARIES=y
CONFIG_APP_BUILD_BOOTLOADER=y
CONFIG_APP_BUILD_USE_FLASH_SECTIONS=y
# CONFIG_APP_REPRODUCIBLE_BUILD is not set
# CONFIG_APP_NO_BLOBS is not set
# end of Build type

#
# Bootloader config
#

#
# Bootloader manager
#
CONFIG_BOOTLOADER_COMPILE_TIME_DATE=y
CONFIG_BOOTLOADER_PROJECT_VER=1
# end of Bootloader manager

#
# Application Rollback
#
# CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE is not set
# end of Application Rollback

#
# Recovery Bootloader and Rollback
#
# end of Recovery Bootloader and Rollback

CONFIG_BOOTLOADER_OFFSET_IN_FLASH=0x0
CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE=y
# CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG is not set
# CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF is not set
# CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE is not set

#
# Log
#
CONFIG_BOOTLOADER_LOG_VERSION_1=y
CONFIG_BOOTLOADER_LOG_VERSION=1
# CONFIG_BOOTLOADER_LOG_LEVEL_NONE is not set
# CONFIG_BOOTLOADER_LOG_LEVEL_ERROR is not set
# CONFIG_BOOTLOADER_LOG_LEVEL_WARN is not set
CONFIG_BOOTLOADER_LOG_LEVEL_INFO=y
# CONFIG_BOOTLOADER_LOG_LEVEL_DEBUG is not set
# CONFIG_BOOTLOADER_LOG_LEVEL_VERBOSE is not set
CONFIG_BOOTLOADER_LOG_LEVEL=3

#
# Format
#
# CONFIG_BOOTLOADER_LOG_COLORS is not set
CONFIG_BOOTLOADER_LOG_TIMESTAMP_SOURCE_CPU_TICKS=y
# end of Format

#
# Settings
#
CONFIG_BOOTLOADER_LOG_MODE_TEXT_EN=y
CONFIG_BOOTLOADER_LOG_MODE_TEXT=y
# end of Settings
# end of Log

#
# Serial Flash Configurations
#
# CONFIG_BOOTLOADER_FLASH_DC_AWARE is not set
CONFIG_BOOTLOADER_FLASH_XMC_SUPPORT=y
# end of Serial Flash Configurations

# CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_8V is not set
CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_9V=y
# CONFIG_BOOTLOADER_FACTORY_RESET is not set
# CONFIG_BOOTLOADER_APP_TEST is not set
CONFIG_BOOTLOADER_REGION_PROTECTION_ENABLE=y
CONFIG_BOOTLOADER_WDT_ENABLE=y
# CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE is not set
CONFIG_BOOTLOADER_WDT_TIME_MS=9000
# CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP is not set
# CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON is not set
# CONFIG_BOOTLOADER_SKIP_VALIDATE_ALWAYS is not set
CONFIG_BOOTLOADER_RESERVE_RTC_SIZE=0
# CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC is not set
# end of Bootloader config

#
# Security features
#
CONFIG_SECURE_BOOT_V2_RSA_SUPPORTED=y
CONFIG_SECURE_BOOT_V2_PREFERRED=y
# CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT is not set
# CONFIG_SECURE_BOOT is not set
# CONFIG_SECURE_FLASH_ENC_ENABLED is not set
CONFIG_SECURE_ROM_DL_MODE_ENABLED=y
# end of Security features

#
# Application manager
#
CONFIG_APP_COMPILE_TIME_DATE=y
# CONFIG_APP_EXCLUDE_PROJECT_VER_VAR is not set
# CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR is not set
# CONFIG_APP_PROJECT_VER_FROM_CONFIG is not set
CONFIG_APP_RETRIEVE_LEN_ELF_SHA=9
# end of Application manager

CONFIG_ESP_ROM_HAS_CRC_LE=y
CONFIG_ESP_ROM_HAS_CRC_BE=y
CONFIG_ESP_ROM_HAS_MZ_CRC32=y
CONFIG_ESP_ROM_HAS_JPEG_DECODE=y
CONFIG_ESP_ROM_UART_CLK_IS_XTAL=y
CONFIG_ESP_ROM_HAS_RETARGETABLE_LOCKING=y
CONFIG_ESP_ROM_USB_OTG_NUM=3
CONFIG_ESP_ROM_USB_SERIAL_DEVICE_NUM=4
CONFIG_ESP_ROM_HAS_ERASE_0_REGION_BUG=y
CONFIG_ESP_ROM_HAS_ENCRYPTED_WRITES_USING_LEGACY_DRV=y
CONFIG_ESP_ROM_GET_CLK_FREQ=y
CONFIG_ESP_ROM_HAS_HAL_WDT=y
CONFIG_ESP_ROM_NEEDS_SWSETUP_WORKAROUND=y
CONFIG_ESP_ROM_HAS_LAYOUT_TABLE=y
CONFIG_ESP_ROM_HAS_SPI_FLASH=y
CONFIG_ESP_ROM_HAS_SPI_FLASH_MMAP=y
CONFIG_ESP_ROM_HAS_ETS_PRINTF_BUG=y
CONFIG_ESP_ROM_HAS_NEWLIB=y
CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT=y
CONFIG_ESP_ROM_HAS_NEWLIB_32BIT_TIME=y
CONFIG_ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE=y
CONFIG_ESP_ROM_RAM_APP_NEEDS_MMU_INIT=y
CONFIG_ESP_ROM_HAS_FLASH_COUNT_PAGES_BUG=y
CONFIG_ESP_ROM_HAS_CACHE_SUSPEND_WAITI_BUG=y
CONFIG_ESP_ROM_HAS_CACHE_WRITEBACK_BUG=y
CONFIG_ESP_ROM_HAS_SW_FLOAT=y
CONFIG_ESP_ROM_HAS_VERSION=y
CONFIG_ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB=y
CONFIG_ESP_ROM_HAS_OUTPUT_PUTC_FUNC=y
CONFIG_ESP_ROM_CONSOLE_OUTPUT_SECONDARY=y

#
# Boot ROM Behavior
#
CONFIG_BOOT_ROM_LOG_ALWAYS_ON=y
# CONFIG_BOOT_ROM_LOG_ALWAYS_OFF is not set
# CONFIG_BOOT_ROM_LOG_ON_GPIO_HIGH is not set
# CONFIG_BOOT_ROM_LOG_ON_GPIO_LOW is not set
# end of Boot ROM Behavior

#
# Serial flasher config
#
# CONFIG_ESPTOOLPY_NO_STUB is not set
# CONFIG_ESPTOOLPY_OCT_FLASH is not set
CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=y
CONFIG_ESPTOOLPY_FLASHMODE_QIO=y
# CONFIG_ESPTOOLPY_FLASHMODE_QOUT is not set
# CONFIG_ESPTOOLPY_FLASHMODE_DIO is not set
# CONFIG_ESPTOOLPY_FLASHMODE_DOUT is not set
CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR=y
CONFIG_ESPTOOLPY_FLASHMODE="dio"
# CONFIG_ESPTOOLPY_FLASHFREQ_120M is not set
# CONFIG_ESPTOOLPY_FLASHFREQ_80M is not set
CONFIG_ESPTOOLPY_FLASHFREQ_40M=y
# CONFIG_ESPTOOLPY_FLASHFREQ_20M is not set
CONFIG_ESPTOOLPY_FLASHFREQ="40m"
# CONFIG_ESPTOOLPY_FLASHSIZE_1MB is not set
# CONFIG_ESPTOOLPY_FLASHSIZE_2MB is not set
# CONFIG_ESPTOOLPY_FLASHSIZE_4MB is not set
# CONFIG_ESPTOOLPY_FLASHSIZE_8MB is not set
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
# CONFIG_ESPTOOLPY_FLASHSIZE_32MB is not set
# CONFIG_ESPTOOLPY_FLASHSIZE_64MB is not set
# CONFIG_ESPTOOLPY_FLASHSIZE_128MB is not set
CONFIG_ESPTOOLPY_FLASHSIZE="16MB"
# CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE is not set
CONFIG_ESPTOOLPY_BEFORE_RESET=y
# CONFIG_ESPTOOLPY_BEFORE_NORESET is not set
CONFIG_ESPTOOLPY_BEFORE="default_reset"
CONFIG_ESPTOOLPY_AFTER_RESET=y
# CONFIG_ESPTOOLPY_AFTER_NORESET is not set
CONFIG_ESPTOOLPY_AFTER="hard_reset"
CONFIG_ESPTOOLPY_MONITOR_BAUD=115200
# end of Serial flasher config

#
# Partition Table
#
CONFIG_PARTITION_TABLE_SINGLE_APP=y
# CONFIG_PARTITION_TABLE_SINGLE_APP_LARGE is not set
# CONFIG_PARTITION_TABLE_TWO_OTA is not set
# CONFIG_PARTITION_TABLE_TWO_OTA_LARGE is not set
# CONFIG_PARTITION_TABLE_CUSTOM is not set
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
CONFIG_PARTITION_TABLE_FILENAME="partitions_singleapp.csv"
CONFIG_PARTITION_TABLE_OFFSET=0x8000
CONFIG_PARTITION_TABLE_MD5=y
# end of Partition Table

#
# Compiler options
#
CONFIG_COMPILER_OPTIMIZATION_DEBUG=y
# CONFIG_COMPILER_OPTIMIZATION_SIZE is not set
# CONFIG_COMPILER_OPTIMIZATION_PERF is not set
# CONFIG_COMPILER_OPTIMIZATION_NONE is not set
CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_ENABLE=y
# CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT is not set
# CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE is not set
CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE=y
CONFIG_COMPILER_FLOAT_LIB_FROM_GCCLIB=y
CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL=2
# CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT is not set
CONFIG_COMPILER_HIDE_PATHS_MACROS=y
# CONFIG_COMPILER_CXX_EXCEPTIONS is not set
# CONFIG_COMPILER_CXX_RTTI is not set
CONFIG_COMPILER_STACK_CHECK_MODE_NONE=y
# CONFIG_COMPILER_STACK_CHECK_MODE_NORM is not set
# CONFIG_COMPILER_STACK_CHECK_MODE_STRONG is not set
# CONFIG_COMPILER_STACK_CHECK_MODE_ALL is not set
# CONFIG_COMPILER_NO_MERGE_CONSTANTS is not set
# CONFIG_COMPILER_WARN_WRITE_STRINGS is not set
CONFIG_COMPILER_DISABLE_DEFAULT_ERRORS=y
# CONFIG_COMPILER_DISABLE_GCC12_WARNINGS is not set
# CONFIG_COMPILER_DISABLE_GCC13_WARNINGS is not set
# CONFIG_COMPILER_DISABLE_GCC14_WARNINGS is not set
# CONFIG_COMPILER_DUMP_RTL_FILES is not set
CONFIG_COMPILER_RT_LIB_GCCLIB=y
CONFIG_COMPILER_RT_LIB_NAME="gcc"
CONFIG_COMPILER_ORPHAN_SECTIONS_WARNING=y
# CONFIG_COMPILER_ORPHAN_SECTIONS_PLACE is not set
# CONFIG_COMPILER_STATIC_ANALYZER is not set
# end of Compiler options

#
# Component config
#

#
# Application Level Tracing
#
# CONFIG_APPTRACE_DEST_JTAG is not set
CONFIG_APPTRACE_DEST_NONE=y
# CONFIG_APPTRACE_DEST_UART1 is not set
# CONFIG_APPTRACE_DEST_UART2 is not set
# CONFIG_APPTRACE_DEST_USB_CDC is not set
CONFIG_APPTRACE_DEST_UART_NONE=y
CONFIG_APPTRACE_UART_TASK_PRIO=1
CONFIG_APPTRACE_LOCK_ENABLE=y
# end of Application Level Tracing

#
# Bluetooth
#
# CONFIG_BT_ENABLED is not set

#
# Common Options
#

#
# BLE Log
#
# CONFIG_BLE_LOG_ENABLED is not set
# end of BLE Log

# CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED is not set
# CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED is not set
# CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED is not set
# end of Common Options
# end of Bluetooth

#
# Console Library
#
# CONFIG_CONSOLE_SORTED_HELP is not set
# end of Console Library

#
# Driver Configurations
#

#
# Legacy TWAI Driver Configurations
#
# CONFIG_TWAI_SKIP_LEGACY_CONFLICT_CHECK is not set
CONFIG_TWAI_ERRATA_FIX_LISTEN_ONLY_DOM=y
# end of Legacy TWAI Driver Configurations

#
# Legacy ADC Driver Configuration
#
# CONFIG_ADC_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_ADC_SKIP_LEGACY_CONFLICT_CHECK is not set

#
# Legacy ADC Calibration Configuration
#
# CONFIG_ADC_CALI_SUPPRESS_DEPRECATE_WARN is not set
# end of Legacy ADC Calibration Configuration
# end of Legacy ADC Driver Configuration

#
# Legacy MCPWM Driver Configurations
#
# CONFIG_MCPWM_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_MCPWM_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy MCPWM Driver Configurations

#
# Legacy Timer Group Driver Configurations
#
# CONFIG_GPTIMER_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_GPTIMER_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy Timer Group Driver Configurations

#
# Legacy RMT Driver Configurations
#
# CONFIG_RMT_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_RMT_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy RMT Driver Configurations

#
# Legacy I2S Driver Configurations
#
# CONFIG_I2S_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_I2S_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy I2S Driver Configurations

#
# Legacy I2C Driver Configurations
#
# CONFIG_I2C_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy I2C Driver Configurations

#
# Legacy PCNT Driver Configurations
#
# CONFIG_PCNT_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_PCNT_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy PCNT Driver Configurations

#
# Legacy SDM Driver Configurations
#
# CONFIG_SDM_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_SDM_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy SDM Driver Configurations

#
# Legacy Temperature Sensor Driver Configurations
#
# CONFIG_TEMP_SENSOR_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_TEMP_SENSOR_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy Temperature Sensor Driver Configurations

#
# Legacy Touch Sensor Driver Configurations
#
# CONFIG_TOUCH_SUPPRESS_DEPRECATE_WARN is not set
# CONFIG_TOUCH_SKIP_LEGACY_CONFLICT_CHECK is not set
# end of Legacy Touch Sensor Driver Configurations
# end of Driver Configurations

#
# eFuse Bit Manager
#
# CONFIG_EFUSE_CUSTOM_TABLE is not set
# CONFIG_EFUSE_VIRTUAL is not set
CONFIG_EFUSE_MAX_BLK_LEN=256
# end of eFuse Bit Manager

#
# ESP-TLS
#
CONFIG_ESP_TLS_USING_MBEDTLS=y
# CONFIG_ESP_TLS_USE_SECURE_ELEMENT is not set
CONFIG_ESP_TLS_USE_DS_PERIPHERAL=y
# CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS is not set
# CONFIG_ESP_TLS_SERVER_SESSION_TICKETS is not set
# CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK is not set
# CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL is not set
# CONFIG_ESP_TLS_PSK_VERIFICATION is not set
# CONFIG_ESP_TLS_INSECURE is not set
CONFIG_ESP_TLS_DYN_BUF_STRATEGY_SUPPORTED=y
# end of ESP-TLS

#
# ADC and ADC Calibration
#
# CONFIG_ADC_ONESHOT_CTRL_FUNC_IN_IRAM is not set
# CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE is not set
# CONFIG_ADC_CONTINUOUS_FORCE_USE_ADC2_ON_C3_S3 is not set
# CONFIG_ADC_ENABLE_DEBUG_LOG is not set
# end of ADC and ADC Calibration

#
# Wireless Coexistence
#
CONFIG_ESP_COEX_ENABLED=y
# CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE is not set
# CONFIG_ESP_COEX_GPIO_DEBUG is not set
# end of Wireless Coexistence

#
# Common ESP-related
#
CONFIG_ESP_ERR_TO_NAME_LOOKUP=y
# end of Common ESP-related

#
# ESP-Driver:Camera Controller Configurations
#
# CONFIG_CAM_CTLR_DVP_CAM_ISR_CACHE_SAFE is not set
# end of ESP-Driver:Camera Controller Configurations

#
# ESP-Driver:GPIO Configurations
#
# CONFIG_GPIO_CTRL_FUNC_IN_IRAM is not set
# end of ESP-Driver:GPIO Configurations

#
# ESP-Driver:GPTimer Configurations
#
CONFIG_GPTIMER_ISR_HANDLER_IN_IRAM=y
# CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM is not set
# CONFIG_GPTIMER_ISR_CACHE_SAFE is not set
CONFIG_GPTIMER_OBJ_CACHE_SAFE=y
# CONFIG_GPTIMER_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:GPTimer Configurations

#
# ESP-Driver:I2C Configurations
#
# CONFIG_I2C_ISR_IRAM_SAFE is not set
# CONFIG_I2C_ENABLE_DEBUG_LOG is not set
# CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2 is not set
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM=y
# end of ESP-Driver:I2C Configurations

#
# ESP-Driver:I2S Configurations
#
# CONFIG_I2S_ISR_IRAM_SAFE is not set
# CONFIG_I2S_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:I2S Configurations

#
# ESP-Driver:LEDC Configurations
#
# CONFIG_LEDC_CTRL_FUNC_IN_IRAM is not set
# end of ESP-Driver:LEDC Configurations

#
# ESP-Driver:MCPWM Configurations
#
CONFIG_MCPWM_ISR_HANDLER_IN_IRAM=y
# CONFIG_MCPWM_ISR_CACHE_SAFE is not set
# CONFIG_MCPWM_CTRL_FUNC_IN_IRAM is not set
CONFIG_MCPWM_OBJ_CACHE_SAFE=y
# CONFIG_MCPWM_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:MCPWM Configurations

#
# ESP-Driver:PCNT Configurations
#
# CONFIG_PCNT_CTRL_FUNC_IN_IRAM is not set
# CONFIG_PCNT_ISR_IRAM_SAFE is not set
# CONFIG_PCNT_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:PCNT Configurations

#
# ESP-Driver:RMT Configurations
#
CONFIG_RMT_ENCODER_FUNC_IN_IRAM=y
CONFIG_RMT_TX_ISR_HANDLER_IN_IRAM=y
CONFIG_RMT_RX_ISR_HANDLER_IN_IRAM=y
# CONFIG_RMT_RECV_FUNC_IN_IRAM is not set
# CONFIG_RMT_TX_ISR_CACHE_SAFE is not set
# CONFIG_RMT_RX_ISR_CACHE_SAFE is not set
CONFIG_RMT_OBJ_CACHE_SAFE=y
# CONFIG_RMT_ENABLE_DEBUG_LOG is not set
# CONFIG_RMT_ISR_IRAM_SAFE is not set
# end of ESP-Driver:RMT Configurations

#
# ESP-Driver:Sigma Delta Modulator Configurations
#
# CONFIG_SDM_CTRL_FUNC_IN_IRAM is not set
# CONFIG_SDM_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:Sigma Delta Modulator Configurations

#
# ESP-Driver:SPI Configurations
#
# CONFIG_SPI_MASTER_IN_IRAM is not set
CONFIG_SPI_MASTER_ISR_IN_IRAM=y
# CONFIG_SPI_SLAVE_IN_IRAM is not set
CONFIG_SPI_SLAVE_ISR_IN_IRAM=y
# end of ESP-Driver:SPI Configurations

#
# ESP-Driver:Touch Sensor Configurations
#
# CONFIG_TOUCH_CTRL_FUNC_IN_IRAM is not set
# CONFIG_TOUCH_ISR_IRAM_SAFE is not set
# CONFIG_TOUCH_ENABLE_DEBUG_LOG is not set
# CONFIG_TOUCH_SKIP_FSM_CHECK is not set
# end of ESP-Driver:Touch Sensor Configurations

#
# ESP-Driver:Temperature Sensor Configurations
#
# CONFIG_TEMP_SENSOR_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:Temperature Sensor Configurations

#
# ESP-Driver:TWAI Configurations
#
# CONFIG_TWAI_ISR_IN_IRAM is not set
# CONFIG_TWAI_IO_FUNC_IN_IRAM is not set
# CONFIG_TWAI_ISR_CACHE_SAFE is not set
# CONFIG_TWAI_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:TWAI Configurations

#
# ESP-Driver:UART Configurations
#
# CONFIG_UART_ISR_IN_IRAM is not set
# end of ESP-Driver:UART Configurations

#
# ESP-Driver:UHCI Configurations
#
# CONFIG_UHCI_ISR_HANDLER_IN_IRAM is not set
# CONFIG_UHCI_ISR_CACHE_SAFE is not set
# CONFIG_UHCI_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:UHCI Configurations

#
# ESP-Driver:USB Serial/JTAG Configuration
#
CONFIG_USJ_ENABLE_USB_SERIAL_JTAG=y
# end of ESP-Driver:USB Serial/JTAG Configuration

#
# Ethernet
#
CONFIG_ETH_ENABLED=y
CONFIG_ETH_USE_SPI_ETHERNET=y
# CONFIG_ETH_SPI_ETHERNET_DM9051 is not set
# CONFIG_ETH_SPI_ETHERNET_W5500 is not set
# CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL is not set
# CONFIG_ETH_USE_OPENETH is not set
# CONFIG_ETH_TRANSMIT_MUTEX is not set
# end of Ethernet

#
# Event Loop Library
#
# CONFIG_ESP_EVENT_LOOP_PROFILING is not set
CONFIG_ESP_EVENT_POST_FROM_ISR=y
CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR=y
# end of Event Loop Library

#
# GDB Stub
#
CONFIG_ESP_GDBSTUB_ENABLED=y
# CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME is not set
CONFIG_ESP_GDBSTUB_SUPPORT_TASKS=y
CONFIG_ESP_GDBSTUB_MAX_TASKS=32
# end of GDB Stub

#
# ESP HID
#
CONFIG_ESPHID_TASK_SIZE_BT=2048
CONFIG_ESPHID_TASK_SIZE_BLE=4096
# end of ESP HID

#
# ESP HTTP client
#
CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS=y
# CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH is not set
# CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH is not set
# CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT is not set
CONFIG_ESP_HTTP_CLIENT_EVENT_POST_TIMEOUT=2000
# end of ESP HTTP client

#
# HTTP Server
#
CONFIG_HTTPD_MAX_REQ_HDR_LEN=1024
CONFIG_HTTPD_MAX_URI_LEN=512
CONFIG_HTTPD_ERR_RESP_NO_DELAY=y
CONFIG_HTTPD_PURGE_BUF_LEN=32
# CONFIG_HTTPD_LOG_PURGE_DATA is not set
# CONFIG_HTTPD_WS_SUPPORT is not set
# CONFIG_HTTPD_QUEUE_WORK_BLOCKING is not set
CONFIG_HTTPD_SERVER_EVENT_POST_TIMEOUT=2000
# end of HTTP Server

#
# ESP HTTPS OTA
#
# CONFIG_ESP_HTTPS_OTA_DECRYPT_CB is not set
# CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP is not set
CONFIG_ESP_HTTPS_OTA_EVENT_POST_TIMEOUT=2000
# end of ESP HTTPS OTA

#
# ESP HTTPS server
#
# CONFIG_ESP_HTTPS_SERVER_ENABLE is not set
CONFIG_ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT=2000
# CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK is not set
# end of ESP HTTPS server

#
# Hardware Settings
#
CONFIG_ESP_HW_SUPPORT_FUNC_IN_IRAM=y

#
# Chip revision
#
CONFIG_ESP32S3_REV_MIN_0=y
# CONFIG_ESP32S3_REV_MIN_1 is not set
# CONFIG_ESP32S3_REV_MIN_2 is not set
CONFIG_ESP32S3_REV_MIN_FULL=0
CONFIG_ESP_REV_MIN_FULL=0

#
# Maximum Supported ESP32-S3 Revision (Rev v0.99)
#
CONFIG_ESP32S3_REV_MAX_FULL=99
CONFIG_ESP_REV_MAX_FULL=99
CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL=0
CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL=199

#
# Maximum Supported ESP32-S3 eFuse Block Revision (eFuse Block Rev v1.99)
#
# end of Chip revision

#
# MAC Config
#
CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_STA=y
CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP=y
CONFIG_ESP_MAC_ADDR_UNIVERSE_BT=y
CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH=y
CONFIG_ESP_MAC_UNIVERSAL_MAC_ADDRESSES_FOUR=y
CONFIG_ESP_MAC_UNIVERSAL_MAC_ADDRESSES=4
# CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES_TWO is not set
CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES_FOUR=y
CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES=4
# CONFIG_ESP_MAC_USE_CUSTOM_MAC_AS_BASE_MAC is not set
# end of MAC Config

#
# Sleep Config
#
# CONFIG_ESP_SLEEP_POWER_DOWN_FLASH is not set
CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND=y
CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU=y
CONFIG_ESP_SLEEP_RTC_BUS_ISO_WORKAROUND=y
CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND=y
CONFIG_ESP_SLEEP_WAIT_FLASH_READY_EXTRA_DELAY=2000
# CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION is not set
# CONFIG_ESP_SLEEP_DEBUG is not set
CONFIG_ESP_SLEEP_GPIO_ENABLE_INTERNAL_RESISTORS=y
# end of Sleep Config

#
# RTC Clock Config
#
CONFIG_RTC_CLK_SRC_INT_RC=y
# CONFIG_RTC_CLK_SRC_EXT_CRYS is not set
# CONFIG_RTC_CLK_SRC_EXT_OSC is not set
# CONFIG_RTC_CLK_SRC_INT_8MD256 is not set
CONFIG_RTC_CLK_CAL_CYCLES=1024
CONFIG_RTC_CLK_FUNC_IN_IRAM=y
CONFIG_RTC_TIME_FUNC_IN_IRAM=y
# end of RTC Clock Config

#
# Peripheral Control
#
CONFIG_ESP_PERIPH_CTRL_FUNC_IN_IRAM=y
CONFIG_ESP_REGI2C_CTRL_FUNC_IN_IRAM=y
# end of Peripheral Control

#
# GDMA Configurations
#
CONFIG_GDMA_CTRL_FUNC_IN_IRAM=y
CONFIG_GDMA_ISR_HANDLER_IN_IRAM=y
CONFIG_GDMA_OBJ_DRAM_SAFE=y
# CONFIG_GDMA_ENABLE_DEBUG_LOG is not set
# CONFIG_GDMA_ISR_IRAM_SAFE is not set
# end of GDMA Configurations

#
# Main XTAL Config
#
CONFIG_XTAL_FREQ_40=y
CONFIG_XTAL_FREQ=40
# end of Main XTAL Config

#
# Power Supplier
#

#
# Brownout Detector
#
CONFIG_ESP_BROWNOUT_DET=y
CONFIG_ESP_BROWNOUT_DET_LVL_SEL_7=y
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_6 is not set
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_5 is not set
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_4 is not set
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_3 is not set
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_2 is not set
# CONFIG_ESP_BROWNOUT_DET_LVL_SEL_1 is not set
CONFIG_ESP_BROWNOUT_DET_LVL=7
CONFIG_ESP_BROWNOUT_USE_INTR=y
# end of Brownout Detector
# end of Power Supplier

CONFIG_ESP_SPI_BUS_LOCK_ISR_FUNCS_IN_IRAM=y
CONFIG_ESP_INTR_IN_IRAM=y
# end of Hardware Settings

#
# ESP-Driver:LCD Controller Configurations
#
# CONFIG_LCD_RGB_ISR_IRAM_SAFE is not set
# CONFIG_LCD_RGB_RESTART_IN_VSYNC is not set
# CONFIG_LCD_ENABLE_DEBUG_LOG is not set
# end of ESP-Driver:LCD Controller Configurations

#
# ESP-MM: Memory Management Configurations
#
# CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS is not set
# end of ESP-MM: Memory Management Configurations

#
# ESP NETIF Adapter
#
CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL=120
# CONFIG_ESP_NETIF_PROVIDE_CUSTOM_IMPLEMENTATION is not set
CONFIG_ESP_NETIF_TCPIP_LWIP=y
# CONFIG_ESP_NETIF_LOOPBACK is not set
CONFIG_ESP_NETIF_USES_TCPIP_WITH_BSD_API=y
CONFIG_ESP_NETIF_REPORT_DATA_TRAFFIC=y
# CONFIG_ESP_NETIF_RECEIVE_REPORT_ERRORS is not set
# CONFIG_ESP_NETIF_L2_TAP is not set
# CONFIG_ESP_NETIF_BRIDGE_EN is not set
# CONFIG_ESP_NETIF_SET_DNS_PER_DEFAULT_NETIF is not set
# end of ESP NETIF Adapter

#
# Partition API Configuration
#
# end of Partition API Configuration

#
# PHY
#
CONFIG_ESP_PHY_ENABLED=y
CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE=y
# CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION is not set
CONFIG_ESP_PHY_MAX_WIFI_TX_POWER=20
CONFIG_ESP_PHY_MAX_TX_POWER=20
# CONFIG_ESP_PHY_REDUCE_TX_POWER is not set
CONFIG_ESP_PHY_ENABLE_USB=y
# CONFIG_ESP_PHY_ENABLE_CERT_TEST is not set
CONFIG_ESP_PHY_RF_CAL_PARTIAL=y
# CONFIG_ESP_PHY_RF_CAL_NONE is not set
# CONFIG_ESP_PHY_RF_CAL_FULL is not set
CONFIG_ESP_PHY_CALIBRATION_MODE=0
CONFIG_ESP_PHY_PLL_TRACK_PERIOD_MS=1000
# CONFIG_ESP_PHY_PLL_TRACK_DEBUG is not set
# CONFIG_ESP_PHY_RECORD_USED_TIME is not set
CONFIG_ESP_PHY_IRAM_OPT=y
# CONFIG_ESP_PHY_DEBUG is not set
# end of PHY

#
# Power Management
#
# CONFIG_PM_SLEEP_FUNC_IN_IRAM is not set
# CONFIG_PM_ENABLE is not set
# CONFIG_PM_SLP_IRAM_OPT is not set
CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP=y
CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP=y
# end of Power Management

#
# ESP PSRAM
#
# CONFIG_SPIRAM is not set
# end of ESP PSRAM

#
# ESP Ringbuf
#
# CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH is not set
# end of ESP Ringbuf

#
# ESP-ROM
#
CONFIG_ESP_ROM_PRINT_IN_IRAM=y
# end of ESP-ROM

#
# ESP Security Specific
#
# end of ESP Security Specific

#
# ESP System Settings
#
# CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_80 is not set
CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_160=y
# CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240 is not set
CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ=160

#
# Cache config
#
CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB=y
# CONFIG_ESP32S3_INSTRUCTION_CACHE_32KB is not set
CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE=0x4000
# CONFIG_ESP32S3_INSTRUCTION_CACHE_4WAYS is not set
CONFIG_ESP32S3_INSTRUCTION_CACHE_8WAYS=y
CONFIG_ESP32S3_ICACHE_ASSOCIATED_WAYS=8
# CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_16B is not set
CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_32B=y
CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_SIZE=32
# CONFIG_ESP32S3_DATA_CACHE_16KB is not set
CONFIG_ESP32S3_DATA_CACHE_32KB=y
# CONFIG_ESP32S3_DATA_CACHE_64KB is not set
CONFIG_ESP32S3_DATA_CACHE_SIZE=0x8000
# CONFIG_ESP32S3_DATA_CACHE_4WAYS is not set
CONFIG_ESP32S3_DATA_CACHE_8WAYS=y
CONFIG_ESP32S3_DCACHE_ASSOCIATED_WAYS=8
# CONFIG_ESP32S3_DATA_CACHE_LINE_16B is not set
CONFIG_ESP32S3_DATA_CACHE_LINE_32B=y
# CONFIG_ESP32S3_DATA_CACHE_LINE_64B is not set
CONFIG_ESP32S3_DATA_CACHE_LINE_SIZE=32
# end of Cache config

#
# Memory
#
# CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM is not set
# CONFIG_ESP32S3_USE_FIXED_STATIC_RAM_SIZE is not set
# end of Memory

#
# Trace memory
#
# CONFIG_ESP32S3_TRAX is not set
CONFIG_ESP32S3_TRACEMEM_RESERVE_DRAM=0x0
# end of Trace memory

CONFIG_ESP_SYSTEM_IN_IRAM=y
# CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT is not set
CONFIG_ESP_SYSTEM_PANIC_PRINT_REBOOT=y
# CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT is not set
# CONFIG_ESP_SYSTEM_PANIC_GDBSTUB is not set
CONFIG_ESP_SYSTEM_PANIC_REBOOT_DELAY_SECONDS=0
CONFIG_ESP_SYSTEM_RTC_FAST_MEM_AS_HEAP_DEPCHECK=y
CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP=y

#
# Memory protection
#
CONFIG_ESP_SYSTEM_MEMPROT_FEATURE=y
CONFIG_ESP_SYSTEM_MEMPROT_FEATURE_LOCK=y
# end of Memory protection

CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE=32
CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE=2304
CONFIG_ESP_MAIN_TASK_STACK_SIZE=4096
CONFIG_ESP_MAIN_TASK_AFFINITY_CPU0=y
# CONFIG_ESP_MAIN_TASK_AFFINITY_CPU1 is not set
# CONFIG_ESP_MAIN_TASK_AFFINITY_NO_AFFINITY is not set
CONFIG_ESP_MAIN_TASK_AFFINITY=0x0
CONFIG_ESP_MINIMAL_SHARED_STACK_SIZE=2048
CONFIG_ESP_CONSOLE_UART_DEFAULT=y
# CONFIG_ESP_CONSOLE_USB_CDC is not set
# CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG is not set
# CONFIG_ESP_CONSOLE_UART_CUSTOM is not set
# CONFIG_ESP_CONSOLE_NONE is not set
# CONFIG_ESP_CONSOLE_SECONDARY_NONE is not set
CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG=y
CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED=y
CONFIG_ESP_CONSOLE_UART=y
CONFIG_ESP_CONSOLE_UART_NUM=0
CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM=0
CONFIG_ESP_CONSOLE_UART_BAUDRATE=115200
CONFIG_ESP_INT_WDT=y
CONFIG_ESP_INT_WDT_TIMEOUT_MS=300
CONFIG_ESP_INT_WDT_CHECK_CPU1=y
CONFIG_ESP_TASK_WDT_EN=y
CONFIG_ESP_TASK_WDT_INIT=y
# CONFIG_ESP_TASK_WDT_PANIC is not set
CONFIG_ESP_TASK_WDT_TIMEOUT_S=5
CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0=y
CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1=y
# CONFIG_ESP_PANIC_HANDLER_IRAM is not set
# CONFIG_ESP_DEBUG_STUBS_ENABLE is not set
CONFIG_ESP_DEBUG_OCDAWARE=y
CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4=y
CONFIG_ESP_SYSTEM_BBPLL_RECALIB=y
# end of ESP System Settings

#
# IPC (Inter-Processor Call)
#
CONFIG_ESP_IPC_ENABLE=y
CONFIG_ESP_IPC_TASK_STACK_SIZE=1280
CONFIG_ESP_IPC_USES_CALLERS_PRIORITY=y
CONFIG_ESP_IPC_ISR_ENABLE=y
# end of IPC (Inter-Processor Call)

#
# ESP Timer (High Resolution Timer)
#
CONFIG_ESP_TIMER_IN_IRAM=y
# CONFIG_ESP_TIMER_PROFILING is not set
CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER=y
CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER=y
CONFIG_ESP_TIMER_TASK_STACK_SIZE=3584
CONFIG_ESP_TIMER_INTERRUPT_LEVEL=1
# CONFIG_ESP_TIMER_SHOW_EXPERIMENTAL is not set
CONFIG_ESP_TIMER_TASK_AFFINITY=0x0
CONFIG_ESP_TIMER_TASK_AFFINITY_CPU0=y
CONFIG_ESP_TIMER_ISR_AFFINITY_CPU0=y
# CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD is not set
CONFIG_ESP_TIMER_IMPL_SYSTIMER=y
# end of ESP Timer (High Resolution Timer)

#
# Wi-Fi
#
CONFIG_ESP_WIFI_ENABLED=y
CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM=10
CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM=32
# CONFIG_ESP_WIFI_STATIC_TX_BUFFER is not set
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER=y
CONFIG_ESP_WIFI_TX_BUFFER_TYPE=1
CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM=32
CONFIG_ESP_WIFI_STATIC_RX_MGMT_BUFFER=y
# CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUFFER is not set
CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF=0
CONFIG_ESP_WIFI_RX_MGMT_BUF_NUM_DEF=5
# CONFIG_ESP_WIFI_CSI_ENABLED is not set
CONFIG_ESP_WIFI_AMPDU_TX_ENABLED=y
CONFIG_ESP_WIFI_TX_BA_WIN=6
CONFIG_ESP_WIFI_AMPDU_RX_ENABLED=y
CONFIG_ESP_WIFI_RX_BA_WIN=6
CONFIG_ESP_WIFI_NVS_ENABLED=y
CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_0=y
# CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_1 is not set
CONFIG_ESP_WIFI_SOFTAP_BEACON_MAX_LEN=752
CONFIG_ESP_WIFI_MGMT_SBUF_NUM=32
CONFIG_ESP_WIFI_IRAM_OPT=y
# CONFIG_ESP_WIFI_EXTRA_IRAM_OPT is not set
CONFIG_ESP_WIFI_RX_IRAM_OPT=y
CONFIG_ESP_WIFI_ENABLE_WPA3_SAE=y
CONFIG_ESP_WIFI_ENABLE_SAE_PK=y
CONFIG_ESP_WIFI_ENABLE_SAE_H2E=y
CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT=y
CONFIG_ESP_WIFI_ENABLE_WPA3_OWE_STA=y
# CONFIG_ESP_WIFI_SLP_IRAM_OPT is not set
CONFIG_ESP_WIFI_SLP_DEFAULT_MIN_ACTIVE_TIME=50
# CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT is not set
CONFIG_ESP_WIFI_SLP_DEFAULT_MAX_ACTIVE_TIME=10
CONFIG_ESP_WIFI_SLP_DEFAULT_WAIT_BROADCAST_DATA_TIME=15
# CONFIG_ESP_WIFI_FTM_ENABLE is not set
CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE=y
# CONFIG_ESP_WIFI_GCMP_SUPPORT is not set
CONFIG_ESP_WIFI_GMAC_SUPPORT=y
CONFIG_ESP_WIFI_SOFTAP_SUPPORT=y
# CONFIG_ESP_WIFI_SLP_BEACON_LOST_OPT is not set
CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM=7
CONFIG_ESP_WIFI_MBEDTLS_CRYPTO=y
CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT=y
# CONFIG_ESP_WIFI_WAPI_PSK is not set
# CONFIG_ESP_WIFI_SUITE_B_192 is not set
# CONFIG_ESP_WIFI_11KV_SUPPORT is not set
# CONFIG_ESP_WIFI_MBO_SUPPORT is not set
# CONFIG_ESP_WIFI_DPP_SUPPORT is not set
# CONFIG_ESP_WIFI_11R_SUPPORT is not set
# CONFIG_ESP_WIFI_WPS_SOFTAP_REGISTRAR is not set

#
# WPS Configuration Options
#
# CONFIG_ESP_WIFI_WPS_STRICT is not set
# CONFIG_ESP_WIFI_WPS_PASSPHRASE is not set
# CONFIG_ESP_WIFI_WPS_RECONNECT_ON_FAIL is not set
# end of WPS Configuration Options

# CONFIG_ESP_WIFI_DEBUG_PRINT is not set
CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT=y
# CONFIG_ESP_WIFI_ENT_FREE_DYNAMIC_BUFFER is not set
# end of Wi-Fi

#
# Core dump
#
# CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH is not set
# CONFIG_ESP_COREDUMP_ENABLE_TO_UART is not set
CONFIG_ESP_COREDUMP_ENABLE_TO_NONE=y
# end of Core dump

#
# FAT Filesystem support
#
CONFIG_FATFS_VOLUME_COUNT=2
CONFIG_FATFS_LFN_NONE=y
# CONFIG_FATFS_LFN_HEAP is not set
# CONFIG_FATFS_LFN_STACK is not set
# CONFIG_FATFS_SECTOR_512 is not set
CONFIG_FATFS_SECTOR_4096=y
# CONFIG_FATFS_CODEPAGE_DYNAMIC is not set
CONFIG_FATFS_CODEPAGE_437=y
# CONFIG_FATFS_CODEPAGE_720 is not set
# CONFIG_FATFS_CODEPAGE_737 is not set
# CONFIG_FATFS_CODEPAGE_771 is not set
# CONFIG_FATFS_CODEPAGE_775 is not set
# CONFIG_FATFS_CODEPAGE_850 is not set
# CONFIG_FATFS_CODEPAGE_852 is not set
# CONFIG_FATFS_CODEPAGE_855 is not set
# CONFIG_FATFS_CODEPAGE_857 is not set
# CONFIG_FATFS_CODEPAGE_860 is not set
# CONFIG_FATFS_CODEPAGE_861 is not set
# CONFIG_FATFS_CODEPAGE_862 is not set
# CONFIG_FATFS_CODEPAGE_863 is not set
# CONFIG_FATFS_CODEPAGE_864 is not set
# CONFIG_FATFS_CODEPAGE_865 is not set
# CONFIG_FATFS_CODEPAGE_866 is not set
# CONFIG_FATFS_CODEPAGE_869 is not set
# CONFIG_FATFS_CODEPAGE_932 is not set
# CONFIG_FATFS_CODEPAGE_936 is not set
# CONFIG_FATFS_CODEPAGE_949 is not set
# CONFIG_FATFS_CODEPAGE_950 is not set
CONFIG_FATFS_CODEPAGE=437
CONFIG_FATFS_FS_LOCK=0
CONFIG_FATFS_TIMEOUT_MS=10000
CONFIG_FATFS_PER_FILE_CACHE=y
# CONFIG_FATFS_USE_FASTSEEK is not set
CONFIG_FATFS_USE_STRFUNC_NONE=y
# CONFIG_FATFS_USE_STRFUNC_WITHOUT_CRLF_CONV is not set
# CONFIG_FATFS_USE_STRFUNC_WITH_CRLF_CONV is not set
CONFIG_FATFS_VFS_FSTAT_BLKSIZE=0
# CONFIG_FATFS_IMMEDIATE_FSYNC is not set
# CONFIG_FATFS_USE_LABEL is not set
CONFIG_FATFS_LINK_LOCK=y
# CONFIG_FATFS_USE_DYN_BUFFERS is not set

#
# File system free space calculation behavior
#
CONFIG_FATFS_DONT_TRUST_FREE_CLUSTER_CNT=0
CONFIG_FATFS_DONT_TRUST_LAST_ALLOC=0
# end of File system free space calculation behavior
# end of FAT Filesystem support

#
# FreeRTOS
#

#
# Kernel
#
# CONFIG_FREERTOS_SMP is not set
# CONFIG_FREERTOS_UNICORE is not set
CONFIG_FREERTOS_HZ=100
# CONFIG_FREERTOS_CHECK_STACKOVERFLOW_NONE is not set
# CONFIG_FREERTOS_CHECK_STACKOVERFLOW_PTRVAL is not set
CONFIG_FREERTOS_CHECK_STACKOVERFLOW_CANARY=y
CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS=1
CONFIG_FREERTOS_IDLE_TASK_STACKSIZE=1536
# CONFIG_FREERTOS_USE_IDLE_HOOK is not set
# CONFIG_FREERTOS_USE_TICK_HOOK is not set
CONFIG_FREERTOS_MAX_TASK_NAME_LEN=16
# CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY is not set
CONFIG_FREERTOS_USE_TIMERS=y
CONFIG_FREERTOS_TIMER_SERVICE_TASK_NAME="Tmr Svc"
# CONFIG_FREERTOS_TIMER_TASK_AFFINITY_CPU0 is not set
# CONFIG_FREERTOS_TIMER_TASK_AFFINITY_CPU1 is not set
CONFIG_FREERTOS_TIMER_TASK_NO_AFFINITY=y
CONFIG_FREERTOS_TIMER_SERVICE_TASK_CORE_AFFINITY=0x7FFFFFFF
CONFIG_FREERTOS_TIMER_TASK_PRIORITY=1
CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH=2048
CONFIG_FREERTOS_TIMER_QUEUE_LENGTH=10
CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE=0
CONFIG_FREERTOS_TASK_NOTIFICATION_ARRAY_ENTRIES=1
# CONFIG_FREERTOS_USE_TRACE_FACILITY is not set
# CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES is not set
# CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS is not set
# CONFIG_FREERTOS_USE_APPLICATION_TASK_TAG is not set
# end of Kernel

#
# Port
#
CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER=y
# CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK is not set
CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS=y
# CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK is not set
# CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP is not set
CONFIG_FREERTOS_CHECK_MUTEX_GIVEN_BY_OWNER=y
CONFIG_FREERTOS_ISR_STACKSIZE=1536
CONFIG_FREERTOS_INTERRUPT_BACKTRACE=y
# CONFIG_FREERTOS_FPU_IN_ISR is not set
CONFIG_FREERTOS_TICK_SUPPORT_SYSTIMER=y
CONFIG_FREERTOS_CORETIMER_SYSTIMER_LVL1=y
# CONFIG_FREERTOS_CORETIMER_SYSTIMER_LVL3 is not set
CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER=y
# CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH is not set
# CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE is not set
# end of Port

#
# Extra
#
# end of Extra

CONFIG_FREERTOS_PORT=y
CONFIG_FREERTOS_NO_AFFINITY=0x7FFFFFFF
CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION=y
CONFIG_FREERTOS_DEBUG_OCDAWARE=y
CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT=y
CONFIG_FREERTOS_PLACE_SNAPSHOT_FUNS_INTO_FLASH=y
CONFIG_FREERTOS_NUMBER_OF_CORES=2
CONFIG_FREERTOS_IN_IRAM=y
# end of FreeRTOS

#
# Hardware Abstraction Layer (HAL) and Low Level (LL)
#
CONFIG_HAL_ASSERTION_EQUALS_SYSTEM=y
# CONFIG_HAL_ASSERTION_DISABLE is not set
# CONFIG_HAL_ASSERTION_SILENT is not set
# CONFIG_HAL_ASSERTION_ENABLE is not set
CONFIG_HAL_DEFAULT_ASSERTION_LEVEL=2
CONFIG_HAL_WDT_USE_ROM_IMPL=y
# end of Hardware Abstraction Layer (HAL) and Low Level (LL)

#
# Heap memory debugging
#
CONFIG_HEAP_POISONING_DISABLED=y
# CONFIG_HEAP_POISONING_LIGHT is not set
# CONFIG_HEAP_POISONING_COMPREHENSIVE is not set
CONFIG_HEAP_TRACING_OFF=y
# CONFIG_HEAP_TRACING_STANDALONE is not set
# CONFIG_HEAP_TRACING_TOHOST is not set
# CONFIG_HEAP_USE_HOOKS is not set
# CONFIG_HEAP_TASK_TRACKING is not set
# CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS is not set
# CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH is not set
# end of Heap memory debugging

#
# Log
#
CONFIG_LOG_VERSION_1=y
# CONFIG_LOG_VERSION_2 is not set
CONFIG_LOG_VERSION=1

#
# Log Level
#
# CONFIG_LOG_DEFAULT_LEVEL_NONE is not set
# CONFIG_LOG_DEFAULT_LEVEL_ERROR is not set
# CONFIG_LOG_DEFAULT_LEVEL_WARN is not set
CONFIG_LOG_DEFAULT_LEVEL_INFO=y
# CONFIG_LOG_DEFAULT_LEVEL_DEBUG is not set
# CONFIG_LOG_DEFAULT_LEVEL_VERBOSE is not set
CONFIG_LOG_DEFAULT_LEVEL=3
CONFIG_LOG_MAXIMUM_EQUALS_DEFAULT=y
# CONFIG_LOG_MAXIMUM_LEVEL_DEBUG is not set
# CONFIG_LOG_MAXIMUM_LEVEL_VERBOSE is not set
CONFIG_LOG_MAXIMUM_LEVEL=3

#
# Level Settings
#
# CONFIG_LOG_MASTER_LEVEL is not set
CONFIG_LOG_DYNAMIC_LEVEL_CONTROL=y
# CONFIG_LOG_TAG_LEVEL_IMPL_NONE is not set
# CONFIG_LOG_TAG_LEVEL_IMPL_LINKED_LIST is not set
CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_AND_LINKED_LIST=y
# CONFIG_LOG_TAG_LEVEL_CACHE_ARRAY is not set
CONFIG_LOG_TAG_LEVEL_CACHE_BINARY_MIN_HEAP=y
CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE=31
# end of Level Settings
# end of Log Level

#
# Format
#
# CONFIG_LOG_COLORS is not set
CONFIG_LOG_TIMESTAMP_SOURCE_RTOS=y
# CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM is not set
# end of Format

#
# Settings
#
CONFIG_LOG_MODE_TEXT_EN=y
CONFIG_LOG_MODE_TEXT=y
# end of Settings

CONFIG_LOG_IN_IRAM=y
# end of Log

#
# LWIP
#
CONFIG_LWIP_ENABLE=y
CONFIG_LWIP_LOCAL_HOSTNAME="espressif"
CONFIG_LWIP_TCPIP_TASK_PRIO=18
# CONFIG_LWIP_TCPIP_CORE_LOCKING is not set
# CONFIG_LWIP_CHECK_THREAD_SAFETY is not set
CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES=y
# CONFIG_LWIP_L2_TO_L3_COPY is not set
# CONFIG_LWIP_IRAM_OPTIMIZATION is not set
# CONFIG_LWIP_EXTRA_IRAM_OPTIMIZATION is not set
CONFIG_LWIP_TIMERS_ONDEMAND=y
CONFIG_LWIP_ND6=y
# CONFIG_LWIP_FORCE_ROUTER_FORWARDING is not set
CONFIG_LWIP_MAX_SOCKETS=10
# CONFIG_LWIP_USE_ONLY_LWIP_SELECT is not set
# CONFIG_LWIP_SO_LINGER is not set
CONFIG_LWIP_SO_REUSE=y
CONFIG_LWIP_SO_REUSE_RXTOALL=y
# CONFIG_LWIP_SO_RCVBUF is not set
# CONFIG_LWIP_NETBUF_RECVINFO is not set
CONFIG_LWIP_IP_DEFAULT_TTL=64
CONFIG_LWIP_IP4_FRAG=y
CONFIG_LWIP_IP6_FRAG=y
# CONFIG_LWIP_IP4_REASSEMBLY is not set
# CONFIG_LWIP_IP6_REASSEMBLY is not set
CONFIG_LWIP_IP_REASS_MAX_PBUFS=10
# CONFIG_LWIP_IP_FORWARD is not set
# CONFIG_LWIP_STATS is not set
CONFIG_LWIP_ESP_GRATUITOUS_ARP=y
CONFIG_LWIP_GARP_TMR_INTERVAL=60
CONFIG_LWIP_ESP_MLDV6_REPORT=y
CONFIG_LWIP_MLDV6_TMR_INTERVAL=40
CONFIG_LWIP_TCPIP_RECVMBOX_SIZE=32
CONFIG_LWIP_DHCP_DOES_ARP_CHECK=y
# CONFIG_LWIP_DHCP_DOES_ACD_CHECK is not set
# CONFIG_LWIP_DHCP_DOES_NOT_CHECK_OFFERED_IP is not set
# CONFIG_LWIP_DHCP_DISABLE_CLIENT_ID is not set
CONFIG_LWIP_DHCP_DISABLE_VENDOR_CLASS_ID=y
# CONFIG_LWIP_DHCP_RESTORE_LAST_IP is not set
CONFIG_LWIP_DHCP_OPTIONS_LEN=69
CONFIG_LWIP_NUM_NETIF_CLIENT_DATA=0
CONFIG_LWIP_DHCP_COARSE_TIMER_SECS=1

#
# DHCP server
#
CONFIG_LWIP_DHCPS=y
CONFIG_LWIP_DHCPS_LEASE_UNIT=60
CONFIG_LWIP_DHCPS_MAX_STATION_NUM=8
CONFIG_LWIP_DHCPS_STATIC_ENTRIES=y
CONFIG_LWIP_DHCPS_ADD_DNS=y
# end of DHCP server

# CONFIG_LWIP_AUTOIP is not set
CONFIG_LWIP_IPV4=y
CONFIG_LWIP_IPV6=y
# CONFIG_LWIP_IPV6_AUTOCONFIG is not set
CONFIG_LWIP_IPV6_NUM_ADDRESSES=3
# CONFIG_LWIP_IPV6_FORWARD is not set
# CONFIG_LWIP_NETIF_STATUS_CALLBACK is not set
CONFIG_LWIP_NETIF_LOOPBACK=y
CONFIG_LWIP_LOOPBACK_MAX_PBUFS=8

#
# TCP
#
CONFIG_LWIP_MAX_ACTIVE_TCP=16
CONFIG_LWIP_MAX_LISTENING_TCP=16
CONFIG_LWIP_TCP_HIGH_SPEED_RETRANSMISSION=y
CONFIG_LWIP_TCP_MAXRTX=12
CONFIG_LWIP_TCP_SYNMAXRTX=12
CONFIG_LWIP_TCP_MSS=1440
CONFIG_LWIP_TCP_TMR_INTERVAL=250
CONFIG_LWIP_TCP_MSL=60000
CONFIG_LWIP_TCP_FIN_WAIT_TIMEOUT=20000
CONFIG_LWIP_TCP_SND_BUF_DEFAULT=5760
CONFIG_LWIP_TCP_WND_DEFAULT=5760
CONFIG_LWIP_TCP_RECVMBOX_SIZE=6
CONFIG_LWIP_TCP_ACCEPTMBOX_SIZE=6
CONFIG_LWIP_TCP_QUEUE_OOSEQ=y
CONFIG_LWIP_TCP_OOSEQ_TIMEOUT=6
CONFIG_LWIP_TCP_OOSEQ_MAX_PBUFS=4
# CONFIG_LWIP_TCP_SACK_OUT is not set
CONFIG_LWIP_TCP_OVERSIZE_MSS=y
# CONFIG_LWIP_TCP_OVERSIZE_QUARTER_MSS is not set
# CONFIG_LWIP_TCP_OVERSIZE_DISABLE is not set
CONFIG_LWIP_TCP_RTO_TIME=1500
# end of TCP

#
# UDP
#
CONFIG_LWIP_MAX_UDP_PCBS=16
CONFIG_LWIP_UDP_RECVMBOX_SIZE=6
# end of UDP

#
# Checksums
#
# CONFIG_LWIP_CHECKSUM_CHECK_IP is not set
# CONFIG_LWIP_CHECKSUM_CHECK_UDP is not set
CONFIG_LWIP_CHECKSUM_CHECK_ICMP=y
# end of Checksums

CONFIG_LWIP_TCPIP_TASK_STACK_SIZE=3072
CONFIG_LWIP_TCPIP_TASK_AFFINITY_NO_AFFINITY=y
# CONFIG_LWIP_TCPIP_TASK_AFFINITY_CPU0 is not set
# CONFIG_LWIP_TCPIP_TASK_AFFINITY_CPU1 is not set
CONFIG_LWIP_TCPIP_TASK_AFFINITY=0x7FFFFFFF
CONFIG_LWIP_IPV6_MEMP_NUM_ND6_QUEUE=3
CONFIG_LWIP_IPV6_ND6_NUM_NEIGHBORS=5
CONFIG_LWIP_IPV6_ND6_NUM_PREFIXES=5
CONFIG_LWIP_IPV6_ND6_NUM_ROUTERS=3
CONFIG_LWIP_IPV6_ND6_NUM_DESTINATIONS=10
# CONFIG_LWIP_IPV6_ND6_ROUTE_INFO_OPTION_SUPPORT is not set
# CONFIG_LWIP_PPP_SUPPORT is not set
# CONFIG_LWIP_SLIP_SUPPORT is not set

#
# ICMP
#
CONFIG_LWIP_ICMP=y
# CONFIG_LWIP_MULTICAST_PING is not set
# CONFIG_LWIP_BROADCAST_PING is not set
# end of ICMP

#
# LWIP RAW API
#
CONFIG_LWIP_MAX_RAW_PCBS=16
# end of LWIP RAW API

#
# SNTP
#
CONFIG_LWIP_SNTP_MAX_SERVERS=1
# CONFIG_LWIP_DHCP_GET_NTP_SRV is not set
CONFIG_LWIP_SNTP_UPDATE_DELAY=3600000
CONFIG_LWIP_SNTP_STARTUP_DELAY=y
CONFIG_LWIP_SNTP_MAXIMUM_STARTUP_DELAY=5000
# end of SNTP

#
# DNS
#
CONFIG_LWIP_DNS_MAX_HOST_IP=1
CONFIG_LWIP_DNS_MAX_SERVERS=3
# CONFIG_LWIP_FALLBACK_DNS_SERVER_SUPPORT is not set
# CONFIG_LWIP_DNS_SETSERVER_WITH_NETIF is not set
# CONFIG_LWIP_USE_ESP_GETADDRINFO is not set
# end of DNS

CONFIG_LWIP_BRIDGEIF_MAX_PORTS=7
CONFIG_LWIP_ESP_LWIP_ASSERT=y

#
# Hooks
#
# CONFIG_LWIP_HOOK_TCP_ISN_NONE is not set
CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT=y
# CONFIG_LWIP_HOOK_TCP_ISN_CUSTOM is not set
CONFIG_LWIP_HOOK_IP6_ROUTE_NONE=y
# CONFIG_LWIP_HOOK_IP6_ROUTE_DEFAULT is not set
# CONFIG_LWIP_HOOK_IP6_ROUTE_CUSTOM is not set
CONFIG_LWIP_HOOK_ND6_GET_GW_NONE=y
# CONFIG_LWIP_HOOK_ND6_GET_GW_DEFAULT is not set
# CONFIG_LWIP_HOOK_ND6_GET_GW_CUSTOM is not set
CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_NONE=y
# CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_DEFAULT is not set
# CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_CUSTOM is not set
CONFIG_LWIP_HOOK_DHCP_EXTRA_OPTION_NONE=y
# CONFIG_LWIP_HOOK_DHCP_EXTRA_OPTION_DEFAULT is not set
# CONFIG_LWIP_HOOK_DHCP_EXTRA_OPTION_CUSTOM is not set
CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_NONE=y
# CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_DEFAULT is not set
# CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM is not set
CONFIG_LWIP_HOOK_DNS_EXT_RESOLVE_NONE=y
# CONFIG_LWIP_HOOK_DNS_EXT_RESOLVE_CUSTOM is not set
# CONFIG_LWIP_HOOK_IP6_INPUT_NONE is not set
CONFIG_LWIP_HOOK_IP6_INPUT_DEFAULT=y
# CONFIG_LWIP_HOOK_IP6_INPUT_CUSTOM is not set
# end of Hooks

# CONFIG_LWIP_DEBUG is not set
# end of LWIP

#
# mbedTLS
#
CONFIG_MBEDTLS_INTERNAL_MEM_ALLOC=y
# CONFIG_MBEDTLS_DEFAULT_MEM_ALLOC is not set
# CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC is not set
CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN=y
CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN=16384
CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN=4096
# CONFIG_MBEDTLS_DYNAMIC_BUFFER is not set
# CONFIG_MBEDTLS_DEBUG is not set

#
# mbedTLS v3.x related
#
# CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 is not set
# CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH is not set
# CONFIG_MBEDTLS_X509_TRUSTED_CERT_CALLBACK is not set
# CONFIG_MBEDTLS_SSL_CONTEXT_SERIALIZATION is not set
CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE=y
# CONFIG_MBEDTLS_SSL_KEYING_MATERIAL_EXPORT is not set
CONFIG_MBEDTLS_PKCS7_C=y
# end of mbedTLS v3.x related

#
# Certificate Bundle
#
CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y
CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL=y
# CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN is not set
# CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_NONE is not set
# CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE is not set
# CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEPRECATED_LIST is not set
CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS=200
# end of Certificate Bundle

# CONFIG_MBEDTLS_ECP_RESTARTABLE is not set
CONFIG_MBEDTLS_CMAC_C=y
CONFIG_MBEDTLS_HARDWARE_AES=y
CONFIG_MBEDTLS_AES_USE_INTERRUPT=y
CONFIG_MBEDTLS_AES_INTERRUPT_LEVEL=0
CONFIG_MBEDTLS_GCM_SUPPORT_NON_AES_CIPHER=y
CONFIG_MBEDTLS_HARDWARE_MPI=y
# CONFIG_MBEDTLS_LARGE_KEY_SOFTWARE_MPI is not set
CONFIG_MBEDTLS_MPI_USE_INTERRUPT=y
CONFIG_MBEDTLS_MPI_INTERRUPT_LEVEL=0
CONFIG_MBEDTLS_HARDWARE_SHA=y
CONFIG_MBEDTLS_ROM_MD5=y
# CONFIG_MBEDTLS_ATCA_HW_ECDSA_SIGN is not set
# CONFIG_MBEDTLS_ATCA_HW_ECDSA_VERIFY is not set
CONFIG_MBEDTLS_HAVE_TIME=y
# CONFIG_MBEDTLS_PLATFORM_TIME_ALT is not set
# CONFIG_MBEDTLS_HAVE_TIME_DATE is not set
CONFIG_MBEDTLS_ECDSA_DETERMINISTIC=y
CONFIG_MBEDTLS_SHA1_C=y
CONFIG_MBEDTLS_SHA512_C=y
# CONFIG_MBEDTLS_SHA3_C is not set
CONFIG_MBEDTLS_TLS_SERVER_AND_CLIENT=y
# CONFIG_MBEDTLS_TLS_SERVER_ONLY is not set
# CONFIG_MBEDTLS_TLS_CLIENT_ONLY is not set
# CONFIG_MBEDTLS_TLS_DISABLED is not set
CONFIG_MBEDTLS_TLS_SERVER=y
CONFIG_MBEDTLS_TLS_CLIENT=y
CONFIG_MBEDTLS_TLS_ENABLED=y

#
# TLS Key Exchange Methods
#
# CONFIG_MBEDTLS_PSK_MODES is not set
CONFIG_MBEDTLS_KEY_EXCHANGE_RSA=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ELLIPTIC_CURVE=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA=y
# end of TLS Key Exchange Methods

CONFIG_MBEDTLS_SSL_RENEGOTIATION=y
CONFIG_MBEDTLS_SSL_PROTO_TLS1_2=y
# CONFIG_MBEDTLS_SSL_PROTO_GMTSSL1_1 is not set
# CONFIG_MBEDTLS_SSL_PROTO_DTLS is not set
CONFIG_MBEDTLS_SSL_ALPN=y
CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS=y
CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS=y

#
# Symmetric Ciphers
#
CONFIG_MBEDTLS_AES_C=y
# CONFIG_MBEDTLS_CAMELLIA_C is not set
# CONFIG_MBEDTLS_DES_C is not set
# CONFIG_MBEDTLS_BLOWFISH_C is not set
# CONFIG_MBEDTLS_XTEA_C is not set
CONFIG_MBEDTLS_CCM_C=y
CONFIG_MBEDTLS_GCM_C=y
# CONFIG_MBEDTLS_NIST_KW_C is not set
# end of Symmetric Ciphers

# CONFIG_MBEDTLS_RIPEMD160_C is not set

#
# Certificates
#
CONFIG_MBEDTLS_PEM_PARSE_C=y
CONFIG_MBEDTLS_PEM_WRITE_C=y
CONFIG_MBEDTLS_X509_CRL_PARSE_C=y
CONFIG_MBEDTLS_X509_CSR_PARSE_C=y
# end of Certificates

CONFIG_MBEDTLS_ECP_C=y
CONFIG_MBEDTLS_PK_PARSE_EC_EXTENDED=y
CONFIG_MBEDTLS_PK_PARSE_EC_COMPRESSED=y
# CONFIG_MBEDTLS_DHM_C is not set
CONFIG_MBEDTLS_ECDH_C=y
CONFIG_MBEDTLS_ECDSA_C=y
# CONFIG_MBEDTLS_ECJPAKE_C is not set
CONFIG_MBEDTLS_ECP_DP_SECP192R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP224R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP192K1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP224K1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP256K1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_BP256R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_BP384R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_BP512R1_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_CURVE25519_ENABLED=y
CONFIG_MBEDTLS_ECP_NIST_OPTIM=y
# CONFIG_MBEDTLS_ECP_FIXED_POINT_OPTIM is not set
# CONFIG_MBEDTLS_POLY1305_C is not set
# CONFIG_MBEDTLS_CHACHA20_C is not set
# CONFIG_MBEDTLS_HKDF_C is not set
# CONFIG_MBEDTLS_THREADING_C is not set
CONFIG_MBEDTLS_ERROR_STRINGS=y
CONFIG_MBEDTLS_FS_IO=y
# CONFIG_MBEDTLS_ALLOW_WEAK_CERTIFICATE_VERIFICATION is not set
# end of mbedTLS

#
# ESP-MQTT Configurations
#
CONFIG_MQTT_PROTOCOL_311=y
# CONFIG_MQTT_PROTOCOL_5 is not set
CONFIG_MQTT_TRANSPORT_SSL=y
CONFIG_MQTT_TRANSPORT_WEBSOCKET=y
CONFIG_MQTT_TRANSPORT_WEBSOCKET_SECURE=y
# CONFIG_MQTT_MSG_ID_INCREMENTAL is not set
# CONFIG_MQTT_SKIP_PUBLISH_IF_DISCONNECTED is not set
# CONFIG_MQTT_REPORT_DELETED_MESSAGES is not set
# CONFIG_MQTT_USE_CUSTOM_CONFIG is not set
# CONFIG_MQTT_TASK_CORE_SELECTION_ENABLED is not set
# CONFIG_MQTT_CUSTOM_OUTBOX is not set
# end of ESP-MQTT Configurations

#
# LibC
#
CONFIG_LIBC_NEWLIB=y
# CONFIG_LIBC_PICOLIBC is not set
CONFIG_LIBC_MISC_IN_IRAM=y
CONFIG_LIBC_LOCKS_PLACE_IN_IRAM=y
CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF=y
# CONFIG_LIBC_STDOUT_LINE_ENDING_LF is not set
# CONFIG_LIBC_STDOUT_LINE_ENDING_CR is not set
# CONFIG_LIBC_STDIN_LINE_ENDING_CRLF is not set
# CONFIG_LIBC_STDIN_LINE_ENDING_LF is not set
CONFIG_LIBC_STDIN_LINE_ENDING_CR=y
# CONFIG_LIBC_NEWLIB_NANO_FORMAT is not set
CONFIG_LIBC_TIME_SYSCALL_USE_RTC_HRT=y
# CONFIG_LIBC_TIME_SYSCALL_USE_RTC is not set
# CONFIG_LIBC_TIME_SYSCALL_USE_HRT is not set
# CONFIG_LIBC_TIME_SYSCALL_USE_NONE is not set
CONFIG_LIBC_ASSERT_BUFFER_SIZE=200
# end of LibC

#
# NVS
#
# CONFIG_NVS_ENCRYPTION is not set
# CONFIG_NVS_ASSERT_ERROR_CHECK is not set
# CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY is not set
# end of NVS

#
# OpenThread
#
# CONFIG_OPENTHREAD_ENABLED is not set

#
# OpenThread Spinel
#
# CONFIG_OPENTHREAD_SPINEL_ONLY is not set
# end of OpenThread Spinel

# CONFIG_OPENTHREAD_DEBUG is not set
# end of OpenThread

#
# Protocomm
#
CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0=y
CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1=y
CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2=y
CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_PATCH_VERSION=y
# end of Protocomm

#
# PThreads
#
CONFIG_PTHREAD_TASK_PRIO_DEFAULT=5
CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT=3072
CONFIG_PTHREAD_STACK_MIN=768
CONFIG_PTHREAD_DEFAULT_CORE_NO_AFFINITY=y
# CONFIG_PTHREAD_DEFAULT_CORE_0 is not set
# CONFIG_PTHREAD_DEFAULT_CORE_1 is not set
CONFIG_PTHREAD_TASK_CORE_DEFAULT=-1
CONFIG_PTHREAD_TASK_NAME_DEFAULT="pthread"
# end of PThreads

#
# MMU Config
#
CONFIG_MMU_PAGE_SIZE_64KB=y
CONFIG_MMU_PAGE_MODE="64KB"
CONFIG_MMU_PAGE_SIZE=0x10000
# end of MMU Config

#
# Main Flash configuration
#

#
# SPI Flash behavior when brownout
#
CONFIG_SPI_FLASH_BROWNOUT_RESET_XMC=y
CONFIG_SPI_FLASH_BROWNOUT_RESET=y
# end of SPI Flash behavior when brownout

#
# Optional and Experimental Features (READ DOCS FIRST)
#

#
# Features here require specific hardware (READ DOCS FIRST!)
#
# CONFIG_SPI_FLASH_HPM_ENA is not set
CONFIG_SPI_FLASH_HPM_AUTO=y
# CONFIG_SPI_FLASH_HPM_DIS is not set
CONFIG_SPI_FLASH_HPM_ON=y
CONFIG_SPI_FLASH_HPM_DC_AUTO=y
# CONFIG_SPI_FLASH_HPM_DC_DISABLE is not set
# CONFIG_SPI_FLASH_AUTO_SUSPEND is not set
CONFIG_SPI_FLASH_SUSPEND_TSUS_VAL_US=50
# CONFIG_SPI_FLASH_FORCE_ENABLE_XMC_C_SUSPEND is not set
# CONFIG_SPI_FLASH_FORCE_ENABLE_C6_H2_SUSPEND is not set
CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM=y
# end of Optional and Experimental Features (READ DOCS FIRST)
# end of Main Flash configuration

#
# SPI Flash driver
#
# CONFIG_SPI_FLASH_VERIFY_WRITE is not set
# CONFIG_SPI_FLASH_ENABLE_COUNTERS is not set
CONFIG_SPI_FLASH_ROM_DRIVER_PATCH=y
# CONFIG_SPI_FLASH_ROM_IMPL is not set
CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS=y
# CONFIG_SPI_FLASH_DANGEROUS_WRITE_FAILS is not set
# CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED is not set
# CONFIG_SPI_FLASH_BYPASS_BLOCK_ERASE is not set
CONFIG_SPI_FLASH_YIELD_DURING_ERASE=y
CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS=20
CONFIG_SPI_FLASH_ERASE_YIELD_TICKS=1
CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE=8192
# CONFIG_SPI_FLASH_SIZE_OVERRIDE is not set
# CONFIG_SPI_FLASH_CHECK_ERASE_TIMEOUT_DISABLED is not set
# CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST is not set

#
# Auto-detect flash chips
#
CONFIG_SPI_FLASH_VENDOR_XMC_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_GD_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_ISSI_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_MXIC_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_WINBOND_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_BOYA_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_VENDOR_TH_SUPPORT_ENABLED=y
CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_MXIC_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_GD_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_WINBOND_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_TH_CHIP=y
CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP=y
# end of Auto-detect flash chips

CONFIG_SPI_FLASH_ENABLE_ENCRYPTED_READ_WRITE=y
# end of SPI Flash driver

#
# SPIFFS Configuration
#
CONFIG_SPIFFS_MAX_PARTITIONS=3

#
# SPIFFS Cache Configuration
#
CONFIG_SPIFFS_CACHE=y
CONFIG_SPIFFS_CACHE_WR=y
# CONFIG_SPIFFS_CACHE_STATS is not set
# end of SPIFFS Cache Configuration

CONFIG_SPIFFS_PAGE_CHECK=y
CONFIG_SPIFFS_GC_MAX_RUNS=10
# CONFIG_SPIFFS_GC_STATS is not set
CONFIG_SPIFFS_PAGE_SIZE=256
CONFIG_SPIFFS_OBJ_NAME_LEN=32
# CONFIG_SPIFFS_FOLLOW_SYMLINKS is not set
CONFIG_SPIFFS_USE_MAGIC=y
CONFIG_SPIFFS_USE_MAGIC_LENGTH=y
CONFIG_SPIFFS_META_LENGTH=4
CONFIG_SPIFFS_USE_MTIME=y

#
# Debug Configuration
#
# CONFIG_SPIFFS_DBG is not set
# CONFIG_SPIFFS_API_DBG is not set
# CONFIG_SPIFFS_GC_DBG is not set
# CONFIG_SPIFFS_CACHE_DBG is not set
# CONFIG_SPIFFS_CHECK_DBG is not set
# CONFIG_SPIFFS_TEST_VISUALISATION is not set
# end of Debug Configuration
# end of SPIFFS Configuration

#
# TCP Transport
#

#
# Websocket
#
CONFIG_WS_TRANSPORT=y
CONFIG_WS_BUFFER_SIZE=1024
# CONFIG_WS_DYNAMIC_BUFFER is not set
# end of Websocket
# end of TCP Transport

#
# Ultra Low Power (ULP) Co-processor
#
# CONFIG_ULP_COPROC_ENABLED is not set

#
# ULP Debugging Options
#
# end of ULP Debugging Options
# end of Ultra Low Power (ULP) Co-processor

#
# Unity unit testing library
#
CONFIG_UNITY_ENABLE_FLOAT=y
CONFIG_UNITY_ENABLE_DOUBLE=y
# CONFIG_UNITY_ENABLE_64BIT is not set
# CONFIG_UNITY_ENABLE_COLOR is not set
CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER=y
# CONFIG_UNITY_ENABLE_FIXTURE is not set
# CONFIG_UNITY_ENABLE_BACKTRACE_ON_FAIL is not set
# CONFIG_UNITY_TEST_ORDER_BY_FILE_PATH_AND_LINE is not set
# end of Unity unit testing library

#
# USB-OTG
#
CONFIG_USB_HOST_CONTROL_TRANSFER_MAX_SIZE=256
CONFIG_USB_HOST_HW_BUFFER_BIAS_BALANCED=y
# CONFIG_USB_HOST_HW_BUFFER_BIAS_IN is not set
# CONFIG_USB_HOST_HW_BUFFER_BIAS_PERIODIC_OUT is not set

#
# Hub Driver Configuration
#

#
# Root Port configuration
#
CONFIG_USB_HOST_DEBOUNCE_DELAY_MS=250
CONFIG_USB_HOST_RESET_HOLD_MS=30
CONFIG_USB_HOST_RESET_RECOVERY_MS=30
CONFIG_USB_HOST_SET_ADDR_RECOVERY_MS=10
# end of Root Port configuration

# CONFIG_USB_HOST_HUBS_SUPPORTED is not set
# end of Hub Driver Configuration

# CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK is not set
CONFIG_USB_OTG_SUPPORTED=y
# end of USB-OTG

#
# Virtual file system
#
CONFIG_VFS_SUPPORT_IO=y
CONFIG_VFS_SUPPORT_DIR=y
CONFIG_VFS_SUPPORT_SELECT=y
CONFIG_VFS_SUPPRESS_SELECT_DEBUG_OUTPUT=y
# CONFIG_VFS_SELECT_IN_RAM is not set
CONFIG_VFS_SUPPORT_TERMIOS=y
CONFIG_VFS_MAX_COUNT=8

#
# Host File System I/O (Semihosting)
#
CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS=1
# end of Host File System I/O (Semihosting)

CONFIG_VFS_INITIALIZE_DEV_NULL=y
# end of Virtual file system

#
# Wear Levelling
#
# CONFIG_WL_SECTOR_SIZE_512 is not set
CONFIG_WL_SECTOR_SIZE_4096=y
CONFIG_WL_SECTOR_SIZE=4096
# end of Wear Levelling

#
# Wi-Fi Provisioning Manager
#
CONFIG_WIFI_PROV_SCAN_MAX_ENTRIES=16
CONFIG_WIFI_PROV_AUTOSTOP_TIMEOUT=30
CONFIG_WIFI_PROV_STA_ALL_CHANNEL_SCAN=y
# CONFIG_WIFI_PROV_STA_FAST_SCAN is not set
# end of Wi-Fi Provisioning Manager
# end of Component config

# CONFIG_IDF_EXPERIMENTAL_FEATURES is not set

# Deprecated options for backward compatibility
# CONFIG_APP_BUILD_TYPE_ELF_RAM is not set
# CONFIG_NO_BLOBS is not set
# CONFIG_APP_ROLLBACK_ENABLE is not set
# CONFIG_LOG_BOOTLOADER_LEVEL_NONE is not set
# CONFIG_LOG_BOOTLOADER_LEVEL_ERROR is not set
# CONFIG_LOG_BOOTLOADER_LEVEL_WARN is not set
CONFIG_LOG_BOOTLOADER_LEVEL_INFO=y
# CONFIG_LOG_BOOTLOADER_LEVEL_DEBUG is not set
# CONFIG_LOG_BOOTLOADER_LEVEL_VERBOSE is not set
CONFIG_LOG_BOOTLOADER_LEVEL=3
# CONFIG_FLASH_ENCRYPTION_ENABLED is not set
CONFIG_FLASHMODE_QIO=y
# CONFIG_FLASHMODE_QOUT is not set
# CONFIG_FLASHMODE_DIO is not set
# CONFIG_FLASHMODE_DOUT is not set
CONFIG_MONITOR_BAUD=115200
CONFIG_OPTIMIZATION_LEVEL_DEBUG=y
CONFIG_COMPILER_OPTIMIZATION_LEVEL_DEBUG=y
CONFIG_COMPILER_OPTIMIZATION_DEFAULT=y
# CONFIG_OPTIMIZATION_LEVEL_RELEASE is not set
# CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE is not set
CONFIG_OPTIMIZATION_ASSERTIONS_ENABLED=y
# CONFIG_OPTIMIZATION_ASSERTIONS_SILENT is not set
# CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED is not set
CONFIG_OPTIMIZATION_ASSERTION_LEVEL=2
# CONFIG_CXX_EXCEPTIONS is not set
CONFIG_STACK_CHECK_NONE=y
# CONFIG_STACK_CHECK_NORM is not set
# CONFIG_STACK_CHECK_STRONG is not set
# CONFIG_STACK_CHECK_ALL is not set
# CONFIG_WARN_WRITE_STRINGS is not set
# CONFIG_ESP32_APPTRACE_DEST_TRAX is not set
CONFIG_ESP32_APPTRACE_DEST_NONE=y
CONFIG_ESP32_APPTRACE_LOCK_ENABLE=y
# CONFIG_EXTERNAL_COEX_ENABLE is not set
# CONFIG_ESP_WIFI_EXTERNAL_COEXIST_ENABLE is not set
# CONFIG_CAM_CTLR_DVP_CAM_ISR_IRAM_SAFE is not set
# CONFIG_GPTIMER_ISR_IRAM_SAFE is not set
# CONFIG_MCPWM_ISR_IRAM_SAFE is not set
# CONFIG_EVENT_LOOP_PROFILING is not set
CONFIG_POST_EVENTS_FROM_ISR=y
CONFIG_POST_EVENTS_FROM_IRAM_ISR=y
CONFIG_GDBSTUB_SUPPORT_TASKS=y
CONFIG_GDBSTUB_MAX_TASKS=32
# CONFIG_OTA_ALLOW_HTTP is not set
# CONFIG_ESP_SYSTEM_PD_FLASH is not set
CONFIG_ESP32S3_DEEP_SLEEP_WAKEUP_DELAY=2000
CONFIG_ESP_SLEEP_DEEP_SLEEP_WAKEUP_DELAY=2000
CONFIG_ESP32S3_RTC_CLK_SRC_INT_RC=y
# CONFIG_ESP32S3_RTC_CLK_SRC_EXT_CRYS is not set
# CONFIG_ESP32S3_RTC_CLK_SRC_EXT_OSC is not set
# CONFIG_ESP32S3_RTC_CLK_SRC_INT_8MD256 is not set
CONFIG_ESP32S3_RTC_CLK_CAL_CYCLES=1024
CONFIG_PERIPH_CTRL_FUNC_IN_IRAM=y
CONFIG_BROWNOUT_DET=y
CONFIG_ESP32S3_BROWNOUT_DET=y
CONFIG_BROWNOUT_DET_LVL_SEL_7=y
CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_7=y
# CONFIG_BROWNOUT_DET_LVL_SEL_6 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_6 is not set
# CONFIG_BROWNOUT_DET_LVL_SEL_5 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_5 is not set
# CONFIG_BROWNOUT_DET_LVL_SEL_4 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_4 is not set
# CONFIG_BROWNOUT_DET_LVL_SEL_3 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_3 is not set
# CONFIG_BROWNOUT_DET_LVL_SEL_2 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_2 is not set
# CONFIG_BROWNOUT_DET_LVL_SEL_1 is not set
# CONFIG_ESP32S3_BROWNOUT_DET_LVL_SEL_1 is not set
CONFIG_BROWNOUT_DET_LVL=7
CONFIG_ESP32S3_BROWNOUT_DET_LVL=7
CONFIG_ESP_SYSTEM_BROWNOUT_INTR=y
CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE=y
# CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION is not set
CONFIG_ESP32_PHY_MAX_WIFI_TX_POWER=20
CONFIG_ESP32_PHY_MAX_TX_POWER=20
# CONFIG_REDUCE_PHY_TX_POWER is not set
# CONFIG_ESP32_REDUCE_PHY_TX_POWER is not set
CONFIG_ESP_SYSTEM_PM_POWER_DOWN_CPU=y
CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP=y
# CONFIG_ESP32S3_SPIRAM_SUPPORT is not set
# CONFIG_ESP32S3_DEFAULT_CPU_FREQ_80 is not set
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_160=y
# CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240 is not set
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ=160
CONFIG_SYSTEM_EVENT_QUEUE_SIZE=32
CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE=2304
CONFIG_MAIN_TASK_STACK_SIZE=4096
CONFIG_CONSOLE_UART_DEFAULT=y
# CONFIG_CONSOLE_UART_CUSTOM is not set
# CONFIG_CONSOLE_UART_NONE is not set
# CONFIG_ESP_CONSOLE_UART_NONE is not set
CONFIG_CONSOLE_UART=y
CONFIG_CONSOLE_UART_NUM=0
CONFIG_CONSOLE_UART_BAUDRATE=115200
CONFIG_INT_WDT=y
CONFIG_INT_WDT_TIMEOUT_MS=300
CONFIG_INT_WDT_CHECK_CPU1=y
CONFIG_TASK_WDT=y
CONFIG_ESP_TASK_WDT=y
# CONFIG_TASK_WDT_PANIC is not set
CONFIG_TASK_WDT_TIMEOUT_S=5
CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0=y
CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1=y
# CONFIG_ESP32_DEBUG_STUBS_ENABLE is not set
CONFIG_ESP32S3_DEBUG_OCDAWARE=y
CONFIG_IPC_TASK_STACK_SIZE=1280
CONFIG_TIMER_TASK_STACK_SIZE=3584
CONFIG_ESP32_WIFI_ENABLED=y
CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM=10
CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM=32
# CONFIG_ESP32_WIFI_STATIC_TX_BUFFER is not set
CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER=y
CONFIG_ESP32_WIFI_TX_BUFFER_TYPE=1
CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM=32
# CONFIG_ESP32_WIFI_CSI_ENABLED is not set
CONFIG_ESP32_WIFI_AMPDU_TX_ENABLED=y
CONFIG_ESP32_WIFI_TX_BA_WIN=6
CONFIG_ESP32_WIFI_AMPDU_RX_ENABLED=y
CONFIG_ESP32_WIFI_RX_BA_WIN=6
CONFIG_ESP32_WIFI_NVS_ENABLED=y
CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_0=y
# CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_1 is not set
CONFIG_ESP32_WIFI_SOFTAP_BEACON_MAX_LEN=752
CONFIG_ESP32_WIFI_MGMT_SBUF_NUM=32
CONFIG_ESP32_WIFI_IRAM_OPT=y
CONFIG_ESP32_WIFI_RX_IRAM_OPT=y
CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE=y
CONFIG_ESP32_WIFI_ENABLE_WPA3_OWE_STA=y
CONFIG_WPA_MBEDTLS_CRYPTO=y
CONFIG_WPA_MBEDTLS_TLS_CLIENT=y
# CONFIG_WPA_WAPI_PSK is not set
# CONFIG_WPA_SUITE_B_192 is not set
# CONFIG_WPA_11KV_SUPPORT is not set
# CONFIG_WPA_MBO_SUPPORT is not set
# CONFIG_WPA_DPP_SUPPORT is not set
# CONFIG_WPA_11R_SUPPORT is not set
# CONFIG_WPA_WPS_SOFTAP_REGISTRAR is not set
# CONFIG_WPA_WPS_STRICT is not set
# CONFIG_WPA_DEBUG_PRINT is not set
# CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH is not set
# CONFIG_ESP32_ENABLE_COREDUMP_TO_UART is not set
CONFIG_ESP32_ENABLE_COREDUMP_TO_NONE=y
CONFIG_TIMER_TASK_PRIORITY=1
CONFIG_TIMER_TASK_STACK_DEPTH=2048
CONFIG_TIMER_QUEUE_LENGTH=10
# CONFIG_ENABLE_STATIC_TASK_CLEAN_UP_HOOK is not set
# CONFIG_HAL_ASSERTION_SILIENT is not set
# CONFIG_L2_TO_L3_COPY is not set
CONFIG_ESP_GRATUITOUS_ARP=y
CONFIG_GARP_TMR_INTERVAL=60
CONFIG_TCPIP_RECVMBOX_SIZE=32
CONFIG_TCP_MAXRTX=12
CONFIG_TCP_SYNMAXRTX=12
CONFIG_TCP_MSS=1440
CONFIG_TCP_MSL=60000
CONFIG_TCP_SND_BUF_DEFAULT=5760
CONFIG_TCP_WND_DEFAULT=5760
CONFIG_TCP_RECVMBOX_SIZE=6
CONFIG_TCP_QUEUE_OOSEQ=y
CONFIG_TCP_OVERSIZE_MSS=y
# CONFIG_TCP_OVERSIZE_QUARTER_MSS is not set
# CONFIG_TCP_OVERSIZE_DISABLE is not set
CONFIG_UDP_RECVMBOX_SIZE=6
CONFIG_TCPIP_TASK_STACK_SIZE=3072
CONFIG_TCPIP_TASK_AFFINITY_NO_AFFINITY=y
# CONFIG_TCPIP_TASK_AFFINITY_CPU0 is not set
# CONFIG_TCPIP_TASK_AFFINITY_CPU1 is not set
CONFIG_TCPIP_TASK_AFFINITY=0x7FFFFFFF
# CONFIG_PPP_SUPPORT is not set
CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF=y
# CONFIG_NEWLIB_STDOUT_LINE_ENDING_LF is not set
# CONFIG_NEWLIB_STDOUT_LINE_ENDING_CR is not set
# CONFIG_NEWLIB_STDIN_LINE_ENDING_CRLF is not set
# CONFIG_NEWLIB_STDIN_LINE_ENDING_LF is not set
CONFIG_NEWLIB_STDIN_LINE_ENDING_CR=y
# CONFIG_NEWLIB_NANO_FORMAT is not set
CONFIG_NEWLIB_TIME_SYSCALL_USE_RTC_HRT=y
CONFIG_ESP32S3_TIME_SYSCALL_USE_RTC_SYSTIMER=y
CONFIG_ESP32S3_TIME_SYSCALL_USE_RTC_FRC1=y
# CONFIG_NEWLIB_TIME_SYSCALL_USE_RTC is not set
# CONFIG_ESP32S3_TIME_SYSCALL_USE_RTC is not set
# CONFIG_NEWLIB_TIME_SYSCALL_USE_HRT is not set
# CONFIG_ESP32S3_TIME_SYSCALL_USE_SYSTIMER is not set
# CONFIG_ESP32S3_TIME_SYSCALL_USE_FRC1 is not set
# CONFIG_NEWLIB_TIME_SYSCALL_USE_NONE is not set
# CONFIG_ESP32S3_TIME_SYSCALL_USE_NONE is not set
CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT=5
CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT=3072
CONFIG_ESP32_PTHREAD_STACK_MIN=768
CONFIG_ESP32_DEFAULT_PTHREAD_CORE_NO_AFFINITY=y
# CONFIG_ESP32_DEFAULT_PTHREAD_CORE_0 is not set
# CONFIG_ESP32_DEFAULT_PTHREAD_CORE_1 is not set
CONFIG_ESP32_PTHREAD_TASK_CORE_DEFAULT=-1
CONFIG_ESP32_PTHREAD_TASK_NAME_DEFAULT="pthread"
CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ABORTS=y
# CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_FAILS is not set
# CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ALLOWED is not set
CONFIG_SUPPRESS_SELECT_DEBUG_OUTPUT=y
CONFIG_SUPPORT_TERMIOS=y
CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS=1
# End of deprecated options
```

## Pinned SDK submodules

Read-only Step 3 inspection confirmed the same 27 pinned revisions. Unrelated SDK untracked projects remain outside these recorded dependencies.

```text
 24c60e243580c7868f4334a1ba3123481fe1aa48 components/bootloader/subproject/components/micro-ecc/micro-ecc (v1.1)
 a348e3fbc75411acd88357af9e1d6a76621c449e components/bt/controller/lib_esp32 (a348e3f)
 1b0f5aac0fcf91ea042c781e6da10a528ca0da03 components/bt/controller/lib_esp32c2/esp32c2-bt-lib (1b0f5aa)
 ebd6043a8e3c3bbde45ee483895303b9c1229ab5 components/bt/controller/lib_esp32c3_family (heads/master)
 1a570a59b6d694b8829c82555f445d88bf26f295 components/bt/controller/lib_esp32c5/esp32c5-bt-lib (1a570a5)
 9d05fc931ae771de52de7e4b4c7090f5b3ec0ec4 components/bt/controller/lib_esp32c6/esp32c6-bt-lib (9d05fc9)
 126dbfeec7e7291458ce6ff8f9297e46f8946733 components/bt/controller/lib_esp32h2/esp32h2-bt-lib (126dbfe)
 e44762384ed104a0ebc78f134eeba5f3ab648ddc components/bt/esp_ble_mesh/lib/lib (heads/main)
 5c1a43ca3d205c3d5183e57dedce335fb6a81155 components/bt/host/nimble/nimble (nimble_1_6_0_tag-398-g5c1a43ca)
 eeecc49ce8af123cf8ad40efdb9673e37b56230f components/cmock/CMock (v2.5.2-2-geeecc49)
 71b47be7c950f1bf5f7e5303779fa99a16224bb6 components/cmock/CMock/vendor/c_exception (v1.3.1-13-g71b47be)
 cf949f45ca6d172a177b00da21310607b97bc7a7 components/cmock/CMock/vendor/unity (v2.5.1)
 ee5cd79583c02f23e43e62931ffb55f5a4992d0f components/esp_coex/lib (ee5cd79)
 3d57415af6e4c92eff2c4c3463e20a51d7340aba components/esp_phy/lib (3d57415)
 b87cce81803b85c3639fb14bac6c24cff6d0fbad components/esp_wifi/lib (b87cce81)
 2867f6883a12920b1969ff9624c0ab0e4185c2ce components/heap/tlsf (2867f68)
 c859b25da02955fef659d658b8f324b5cde87be3 components/json/cJSON (v1.7.15-48-gc859b25)
 fd432e4ee2cfb7f7f1c7eb7227e0173412e7b84e components/lwip/lwip (fd432e4e)
 ffb280bb63c78bfec1e1ab55040671768c85c923 components/mbedtls/mbedtls (mbedtls-3.6.0-1758-gffb280bb6)
 6af4446a48ea7fa54948edc4e62277ed70abb6e1 components/mqtt/esp-mqtt (ESP-MQTT_FOR_IDF_4.x-306-g6af4446)
 2e2d91a382ad6387ce77551c6adbb1514db12472 components/openthread/lib (2e2d91a)
 a12ff0d0f54fd41954b45047fcdd08f302731c5f components/openthread/openthread (thread-reference-20250612-424-ga12ff0d0f)
 e185d7fd85499c8ce5ca2a54f5cf8fe7dbe3f8df components/openthread/openthread/third_party/mbedtls/repo (v3.6.0-1747-ge185d7fd8)
 457996474728cb8e968ed21953b72f74d2f536b2 components/openthread/openthread/third_party/mbedtls/repo/framework (mbedtls-4.0.0_tf-psa-crypto-1.0.0)
 abc67a11c6db271bedbb9f58be85d6f4e2ea8389 components/protobuf-c/protobuf-c (v1.4.1)
 0dbb3f71c5f6fae3747a9d935372773762baf852 components/spiffs/spiffs (0.2-255-g0dbb3f7)
 bf560290f6020737eafaa8b5cbd2177c3956c03f components/unity/unity (v2.6.0_RC1)
```
