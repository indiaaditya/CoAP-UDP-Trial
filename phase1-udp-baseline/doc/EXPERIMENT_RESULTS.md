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
