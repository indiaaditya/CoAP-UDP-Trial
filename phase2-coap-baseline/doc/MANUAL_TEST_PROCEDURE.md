# Phase 2 G4 operator procedure - finalized review candidate

## Authorization and baseline

This procedure is documentation, not execution authorization. No G4 build/test, capture, socket, board query, flash, monitor, repository commit/push, merge or tag is authorized by the watchdog amendment. Commands below are for later explicit G4 authorization only.

Accepted implementation: G3-A aad119d4a980b37e510940e37a85b835a1630717; G3-B c146cb2353feb5cc467f85968fb9f6546fb1f607; G3-C 7dbe6f2298fbdcbeee34669c097ca55648b22f71. The approved one-line host watchdog amendment changes120000 to180000ms; no other implementation behavior changes. It remains uncommitted pending review. Record the eventual approved amendment commit as the execution baseline; do not test an unreviewed dirty source tree.

Use only D:/proff/clients/Welmade/CoAP-UDP-Trial-phase2 on phase2-coap-baseline. Never alter the original Phase1 worktree, its branch, modified Notings.xlsx or untracked Phase2 ESP32.png. No Beken implementation, security, heartbeat, adaptive intervals, push or MQTT handover.

Execution progression: host/environment/build P2-01..08; normal ESP32 P2-09..12; reliability P2-13..18; negative P2-19..25; resource/lifecycle P2-26..28; portability assessment P2-29. No new test IDs or scope.

## Result fields - keep separate

Every test/run record must contain these independent fields:

| Field | Allowed entries / required evidence |
| --- | --- |
| Host fixture validity | VALID / INVALID / N/A; correct mode/CASE, listeners, intended injection, no setup watchdog/limit fault |
| ESP32 communication result | Actual attempts/successes/status/error, or N/A for host-only tests; retain firmware FAIL in a passing negative test |
| Overall test result | PASS / FAIL / BLOCKED / NOT EXECUTED; valid setup plus expected behavior required for PASS |
| Actual packet evidence | Capture filename/interface/packet references, type/code/MID/token/payload/length/retry or ACK observations; missing evidence is not fabricated |
| Runtime measurement validity | VALID / INVALID / N/A, reached B-checkpoints, image/config identities, pool/scope and reasons for missing values |

Examples: correct wrong-code injection plus ESP rejection -> fixture VALID, communication FAIL1/0, overall negative-test PASS. A fixture timing out before the intended observation -> fixture INVALID and overall BLOCKED, not ESP failure. Communication success with missing required capture -> record success but overall BLOCKED. A malformed packet causing bounded failure is a finding/FAIL for the intended recovery test, not automatically PASS. Do not hide failed runs with retries; use a new repetition ID.

## Prerequisites and environment checks (P2-01/P2-05)

- Exact approved source commit, clean Phase2 tracked tree, unchanged Phase1 reference. SDK v5.5.4 commit735507283d5b2f9fb363a1901172dbd9e847945d; GCC14.2.0 esp-14.2.0_20260121; reference Node25.9.0/npm11.12.1.
- Locked coap1.6.0 and direct dev coap-packet1.1.1 installed under phase2-coap-baseline/src/node_modules. Record npm ls; do not upgrade dependencies.
- Same ESP32-S3,16MB QIO40MHz flash, PSRAM disabled, main stack4096, tick100Hz, CPU160MHz, debug optimization,1MiB factory partition.
- Actual serial port identified (historical COM14),115200 baud, stable power/cable, one board. Private network macros available but never printed/copied into evidence.
- Host must own192.168.0.170 and be reachable from the board's intended2.4GHz network. If not, stop; no firmware/IP/firewall/security edits are implicit.
- Ports56830/56831 free before test processes. Stop only test-owned processes; do not kill unrelated Node applications.
- Capture prerequisites below satisfied before dependent hardware cases. Tool readiness is not assumed from installation alone.

After G4 authorization only, record:

```powershell
$Repo = 'D:\proff\clients\Welmade\CoAP-UDP-Trial-phase2'
$Phase = Join-Path $Repo 'phase2-coap-baseline'
$Firmware = Join-Path $Phase 'src\esp32-coap-client'
$Port = 'COM14' # Identify the real board first.
Set-Location $Repo
git branch --show-current
git rev-parse HEAD
git status --short
node --version
npm --version
Get-NetIPAddress -AddressFamily IPv4
Get-NetUDPEndpoint -LocalPort 56830,56831 -ErrorAction SilentlyContinue
```

Stop on unexpected Git state or source/config/lock mismatch. Use only process-local trust settings for verified SDK/worktree paths if Git ownership checks require them, never global configuration changes.

## Evidence layout

Keep evidence outside both Git worktrees; no publication/commit is authorized. After G4 authorization:

```powershell
$BatchId = [DateTimeOffset]::UtcNow.ToOffset([TimeSpan]::FromHours(5.5)).ToString('yyyyMMdd-HHmmss') + '-IST'
$Evidence = Join-Path 'D:\proff\clients\Welmade\CoAP-G4-Evidence' $BatchId
New-Item -ItemType Directory -Path $Evidence
$RunDir = Join-Path $Evidence 'P2-10-primary-R1' # Set per run.
New-Item -ItemType Directory -Path $RunDir
```

Use fresh names, never overwrite a failed run. Examples P2-11-primary-R2, P2-19-blackhole-R1, P2-22-bad-payload-LF-R1, P2-27-scoped-R1. Each folder: run-record.txt, environment.txt, artifact-hashes.txt, server.log, fixture.log, serial.log, capture.pcapng, packet-observations.csv, resource-readings.csv as applicable. Record operator, IST/UTC-offset times, source/fixture/lock identities, authoritative image-set ID, host/board addresses, ports, commands, injected scenario, observations, all five result fields, rerun reason and capture-loss counters. Raw firmware/ELF contain private inputs; keep them local.

## Timing and readiness - approved amendment

FIXTURE_TIMEOUT_MS=180000 from fixture startup. Firmware Wi-Fi budget remains30000ms; firmware application transaction ceiling remains100000ms from initial send. ACK-only does not restart that deadline. Separate delay250ms; duplicate delay250ms; duplicate separate-CON at500ms remain unchanged.

Prepare the serial monitor and capture first; start fixture last, wait for all listening lines, reset promptly. Record a conservative fixture-launch timestamp immediately before the node command, and first ESP front-in request/capture timestamp. First request must arrive within60 seconds of fixture startup (use the earlier launch marker conservatively if exact startup time unavailable). This is an OPERATOR run-validity rule, not a new fixture timer/feature. If no first request by60s, stop the fixture and mark setup INVALID/overall BLOCKED. Retain evidence and do not classify as CoAP failure.

At latest valid first request60s, the100s firmware window ends by approximately160s, leaving20s before the180s host safety ceiling. A93s retry window ends near153s, leaving27s. These allowances include startup before the first request; exceptional host/SDK stalls still invalidate the run if the fixture expires. Watchdog expiry is host setup/fixture failure, not proof of an ESP timeout. Record ESP's actual terminal event independently.

## Packet-capture readiness - mandatory prerequisite

Current readiness: NOT VERIFIED. Earlier preparation found no Wireshark/TShark in PATH/usual installation directory; no installation or capture has occurred. Later authorization must cover any required setup. Do not disable firewall or install tools implicitly.

| Traffic | Capture point / readiness proof |
| --- | --- |
| ESP32 <-> PC | Physical interface carrying192.168.0.170; verify Npcap access and a saved/reopened passive physical capture with correctly identified traffic before hardware tests |
| Fixture <-> backend56831 | Npcap loopback; demonstrate successful capture/decode using approved host P2-07/08 (and corresponding proxy observations), not just a listed adapter |
| Local probe <-> server56830 | Loopback; save/reopen actual approved probe capture |

Identify interface IDs, permissions and address binding. Configure/decode UDP56830 and56831 as CoAP. Keep malformed packets visible. Suitable filter:

```text
udp and (host 192.168.0.170 or port 56830 or port 56831)
```

Unlike a56830-only filter, this retains wrong-port traffic. For P2-24 decode the observed wrong-port conversation as CoAP too. Save/reopen pcapng; verify decoding and no relevant loss. Physical passive readiness must precede P2-09; confirm the actual first ESP frames during P2-09 before allowing P2-10 onward. If readiness/evidence is absent, dependent tests are BLOCKED. Do not generate extra unapproved network traffic to prove capture readiness.

Capture physical plus loopback when relevant; distinguish interface IDs and backend-forwarded packets from device-wire traffic. No backend duplicate counting as ESP retransmission. Record type/code/MID/token/path/format/payload, source/destination and timestamps. CoAP/UDP payload: request24 bytes, piggyback/separate PONG14, empty ACK/RST4. UDP length adds8. Header4+token4+URI-Path10+format1+marker1+PING4=24; previous23 estimate was arithmetic only.

## Authoritative firmware sets - immutable selection

Select exactly these two retained, already build-reviewed G3-B sets for hardware runs unless a replacement pair is separately reviewed/approved. File SHA256 reads during this amendment matched the ledger; no rebuild or hardware verification occurred.

- P2-PRIMARY-G3B: firmware/build-primary; primary option disabled; application907040 bytes. Use for communication/reliability/negative/P2-26 tests.
- P2-SCOPED-G3B: firmware/build-scoped; scoped option enabled; application907984 bytes. Use for P2-27.

The host-only watchdog amendment does not change either firmware set. The accepted firmware provenance is G3-B c146cb2353feb5cc467f85968fb9f6546fb1f607; runtime source/version strings may reflect historical metadata, not the newest host/document commit. Use hashes, not version text alone.

Logical immutability means do not rebuild into/overwrite these selected directories. Rehash all members before flashing and record set ID plus application/config SHA256 in EVERY hardware run. On mismatch stop; do not silently substitute or mix an ELF, config, bootloader or partition file from another build. After flash record application hash again and generated flash_args/flasher_args.json used. A rebuild is a new artifact set even with identical size/source; new hashes cannot inherit this identity. P2-03/04 fresh verification builds below are not automatically promoted to hardware use.

| Set | Member relative to its build directory | Bytes | SHA256 |
| --- | --- | ---: | --- |
| P2-PRIMARY-G3B | esp32_coap_client.bin | 907040 | 4102f0a46436431c9493555a4980e18d308eac40a541bb0fcb1669cda11ac8a9 |
| P2-PRIMARY-G3B | esp32_coap_client.elf | 10071472 | 1ee47ba8c752d1cbddd0fb01899a330e85c775807c5fa297918868bc82e12533 |
| P2-PRIMARY-G3B | esp32_coap_client.map | 8837869 | e6041aca172fee328433306ff02967c4c93736df63d190b2ce2d828f0e1e927f |
| P2-PRIMARY-G3B | bootloader/bootloader.bin | 22272 | e278d249ab684203934f1d294a0917a2ac26becb1e0173f4e6ed51ed34b1eef4 |
| P2-PRIMARY-G3B | partition_table/partition-table.bin | 3072 | 7f00b6c042a89b15b0cac534f82ed988caf29278ff5700b0c511eb1b5bb7c820 |
| P2-PRIMARY-G3B | sdkconfig | 76669 | 3010c3c7901f1744c9027abb1ce1145de9ef25814f1d3f679a2429b751de01e9 |
| P2-SCOPED-G3B | esp32_coap_client.bin | 907984 | a17d628714b87614c1087b2e4fdcc7657a647bffd72c3fe120909b46f0248507 |
| P2-SCOPED-G3B | esp32_coap_client.elf | 10076632 | 4de3133ddf3c0b830da4174773b818bc48d3ee8a88f4b3248e33cc631ca131c2 |
| P2-SCOPED-G3B | esp32_coap_client.map | 8838830 | 9caf7be0a34311733130ea70caafde0c3138cb3cc4ec4250880000a9860631ea |
| P2-SCOPED-G3B | bootloader/bootloader.bin | 22272 | f833d6eb13a00a9b07ddae004428a1521c204349b018b025222b77770e5f5338 |
| P2-SCOPED-G3B | partition_table/partition-table.bin | 3072 | 7f00b6c042a89b15b0cac534f82ed988caf29278ff5700b0c511eb1b5bb7c820 |
| P2-SCOPED-G3B | sdkconfig | 76658 | c05ab44f8fa418b518ce44170ea58508141763cb7f1848570cebde8278ba29e2 |


Verify members using Get-FileHash -Algorithm SHA256; do not print binary contents. Configuration comparison must show only CONFIG_TRIAL_SCOPED_HEAP_MONITOR=n/y; preserve SDK/compiler/optimization/PSRAM/partition/stack/network settings. Source/config/lock hashes and full section evidence are in RESOURCE_BASELINE.md.

## Build verification P2-03/P2-04

Only after G4 build execution authorization; never delete retained outputs. Fresh output directories under evidence, stop if they already exist. SDK export:

```powershell
$env:IDF_TOOLS_PATH = 'C:\Espressif'
$env:PATH = 'C:\Espressif\python_env\idf5.5_py3.11_env\Scripts;' + $env:PATH
. C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1
$PrimaryVerify = Join-Path $Evidence 'P2-03\build-primary'
$ScopedVerify = Join-Path $Evidence 'P2-04\build-scoped'
if ((Test-Path $PrimaryVerify) -or (Test-Path $ScopedVerify)) { throw 'Use fresh build paths.' }
idf.py --no-ccache -C "$Firmware" -B "$PrimaryVerify" -DIDF_TARGET=esp32s3 "-DSDKCONFIG=$PrimaryVerify\sdkconfig" "-DSDKCONFIG_DEFAULTS=$Firmware\sdkconfig.defaults" build
idf.py --no-ccache -C "$Firmware" -B "$ScopedVerify" -DIDF_TARGET=esp32s3 "-DSDKCONFIG=$ScopedVerify\sdkconfig" "-DSDKCONFIG_DEFAULTS=$Firmware\sdkconfig.defaults;$Firmware\sdkconfig.scoped.defaults" build
```

Capture commands/full logs/exit codes and binary/ELF/map/config/bootloader/partition identities. Confirm fit, warnings and normalized configurations. Rebuilt hashes can differ due metadata/time; investigate unexpected sizes/config differences before hardware use. Default authoritative hardware sets remain the retained pair above. Do not optimize or alter source to match size.

P2-02 missing-header check uses a temporary committed-source export outside the repository, not renaming the actual header:

```powershell
$Scratch = Join-Path $Evidence 'P2-02-missing-header'
New-Item -ItemType Directory -Path $Scratch
git -C "$Repo" archive --format=tar "--output=$Scratch\source.tar" 7dbe6f2298fbdcbeee34669c097ca55648b22f71 phase2-coap-baseline/src/esp32-coap-client
tar -xf "$Scratch\source.tar" -C "$Scratch"
$ScratchFirmware = Join-Path $Scratch 'phase2-coap-baseline\src\esp32-coap-client'
idf.py --no-ccache -C "$ScratchFirmware" -B "$Scratch\build" -DIDF_TARGET=esp32s3 reconfigure
```

Expected: exact missing .private/wifi_credentials.h error without secret values. Different configure/dependency errors are BLOCKED, not a passing missing-header result. Firmware source is unchanged since that accepted commit.

## Server/fixture consoles and stop procedure

All commands here remain unexecuted until G4 network authorization. Set RunDir to an existing fresh run folder in each console.

Standard server (56830):

```powershell
Set-Location $Phase
Remove-Item Env:TRIAL_COAP_PORT -ErrorAction SilentlyContinue
node src/cloud.js 2>&1 | Tee-Object -FilePath "$RunDir\server.log"
```

Expect Listening on CoAP UDP0.0.0.0:56830. Proxy backend:

```powershell
$env:TRIAL_COAP_PORT = '56831'
node src/cloud.js 2>&1 | Tee-Object -FilePath "$RunDir\server.log"
```

Expect56831; clear override after stopping: Remove-Item Env:TRIAL_COAP_PORT -ErrorAction SilentlyContinue.

Fixture (omit CASE except where specified):

```powershell
[DateTimeOffset]::UtcNow.ToOffset([TimeSpan]::FromHours(5.5)).ToString('o') | Set-Content "$RunDir\fixture.log"
node test/coap_fixture.js MODE CASE 2>&1 | Tee-Object -Append -FilePath "$RunDir\fixture.log"
```

Launch marker above is conservative for60s readiness. Incoming metadata time is epoch milliseconds; retain capture timestamps too. Wait front/back listening for proxy; front for synthetic; front/wrong-port for wrong-port. Probe binds ephemeral loopback and sends once. No reuse across board resets: fixture locks first client endpoint and reset may change source port. Fresh server avoids prior response-cache state.

| Mode | CASE | Peer / action |
| --- | --- | --- |
| pass | none | Backend56831; forward unchanged |
| drop-first-request | none | Backend56831; first front datagram suppressed |
| drop-first-response | none | Backend56831; first backend datagram suppressed |
| duplicate-request | none | Backend56831; first request immediate plus identical copy250ms later |
| duplicate-response | none | Backend56831; first response immediate plus identical copy250ms later |
| blackhole | none | No standard server56830; log requests, no response |
| separate | none | Empty ACK, CON PONG after250ms/new MID/same token |
| separate-duplicate | none | As separate; identical CON again at500ms |
| ack-only | none | Empty ACK only, never application PONG |
| wrong-token | none | Matching MID, first token byte toggled |
| wrong-code | none | Matching correlation, code4.00 |
| bad-payload | FAIL / LF / OVERSIZE | Default FAIL4 bytes; PONG newline5;100X bytes |
| malformed | none | Truncated3-byte header then valid reply250ms later |
| wrong-port | none | Correct reply from distinct ephemeral socket |
| rst | none | Empty matching-MID RST |
| probe | valid / path / method / payload / format | Standard server56830; expected2.04/4.04/4.05/4.00/4.00 |

Synthetic modes replace the normal56830 server. They are scoped stimuli, not a general server retransmission engine; real-server reliability/cache checks use proxy modes. Preserve all MID/token bytes on loss/duplication.

Ctrl+C in each test-owned console stops it. Fixture should print STOPPED/operator-stop; this is not test PASS. Send-completion log is not delivery proof. Record timeout/capture-limit/socket errors and exit status. If Ctrl+C fails, identify the exact test-owned PID from endpoint/process details before targeted termination; never stop unrelated Node processes. Confirm56830/56831 released. Preserve interrupted-run evidence.

## Flashing and serial - separate later authorization required

Choose SelectedBuild as the authoritative primary or scoped directory, never the fresh verification directory implicitly. Verify hashes/config first, board identity/port/power and existing plaintext-compatible security state. Close serial monitor before flash. A later-authorized read-only security query may reset/enter the bootloader:

```powershell
python -m esptool --chip esp32s3 --port "$Port" --before default_reset --after no_reset get_security_info
```

Stop on security-state conflict. No eFuse, key, force override, whole-chip erase or recovery authorization. Inspect generated flash_args and flasher_args.json; expected bootloader0x0, partition0x8000, app0x10000,1MiB factory. Generated DIO boot header followed by configured QIO bootloader behavior is expected, not a setting to change.

```powershell
$SelectedBuild = Join-Path $Firmware 'build-primary' # Or build-scoped for P2-27.
Get-FileHash -Algorithm SHA256 "$SelectedBuild\esp32_coap_client.bin"
Get-Content "$SelectedBuild\flash_args"
Push-Location $SelectedBuild
try {
    python -m esptool --chip esp32s3 --port "$Port" --baud 115200 --before default_reset --after no_reset write_flash '@flash_args'
    if ($LASTEXITCODE -ne 0) { throw 'Stop: flashing failed.' }
} finally { Pop-Location }
```

Direct generated response-file flashing avoids an implicit rebuild. Record verification output; normal writes erase/overwrite addressed sectors only. After no_reset, attach logging without resetting and then use physical EN only when peers/capture ready:

```powershell
Set-Location $RunDir
python -m esp_idf_monitor -p "$Port" -b 115200 --no-reset --timestamps "$SelectedBuild\esp32_coap_client.elf"
```

Default Ctrl+T then Ctrl+L toggles log file; confirm enabled filename in run directory. Ctrl+] exits. Serial monitor must use matching ELF. Capture and serial logging ready BEFORE fixture launch; listeners ready BEFORE one EN/RESET. Record reset time and first request<=60s. Do not start watchdog before flashing. Any incidental setup boot is labeled setup, not silently included in a qualifying run.

Common hardware run: confirm selected set; previous campaign finished; new evidence folder; serial log; capture; fresh peer(s); readiness; single reset; observe result/return-from-main; stop test peers; save capture/logs; record all five result fields. Never reuse one fixture across reset or aggregate intentional failures as packet loss.

Expected healthy serial forms (actual numbers vary): B1 internal heap before Wi-Fi; IPv4 acquired; B2 internal heap after IPv4; Task counts before_coap/after_coap; B3 internal heap before first exchange; Exchange N POST /coap-test PING mid/token; Exchange N received exact PONG successes=N; Result attempts=20 successes=20 status=PASS; Measurements status=OK; B5 free/B6 minimum stack; primary Whole-run internal heap minimum/B4 unavailable. Scoped adds monitor before/after and B4 campaign-scoped minimum, deliberately no whole-run watermark. Require return-from-main without panic/cleanup warning. Cleanup-generated disconnect after PASS is not automatically exchange failure. Error enums3=NACK,4=I/O,5=deadline,6=response,7=Wi-Fi,8=measurement.

## P2-01 through P2-08: environment/build/host-only network

| ID | Setup and execution | Expected / PASS criterion |
| --- | --- | --- |
| P2-01 | Record source/SDK/tools/locks, board/port identity, selected-set hashes and capture readiness | Exact approved implementation/environment, no unexplained difference. Setup gap ->BLOCKED |
| P2-02 | Scratch export/configure without private header as above | Intended missing-header failure; no secret disclosure; unrelated failure not PASS |
| P2-03 | Fresh primary verification build, record logs/hashes/size/config | Successful build/fit, approved primary option, no project-source warning; no automatic artifact promotion |
| P2-04 | Fresh scoped verification build; compare normalized config | Successful build; only approved option differs; distinct traceable outputs |
| P2-05 | Review tracked/ignored config/private/generated paths, locks, relevant source integrity | No unauthorized configuration/file/dependency change or private data in reviewable evidence |
| P2-06 | node --check src/cloud.js and test/coap_fixture.js; standard56830 server, second server same port | First listens, second reports occupied port and exits unsuccessfully, first remains usable; retain actual error/cleanup output |
| P2-07 | Standard56830 + fresh probe valid with loopback capture | Fixture valid PASS/exit0,2.04/PONG/text0/matching token; actual saved/reopened packet evidence |
| P2-08 | Separate fresh probe path, method, payload, format invocations | Fixture validates4.04,4.05,4.00,4.00 respectively; server remains available; correct empty/error payload as implemented |

Host-only tests have ESP result N/A. Their actual socket/package behavior is not established by earlier mocks. P2-07/08 establish loopback capture readiness before dependent ESP/proxy tests; physical passive readiness also required.

## P2-09 through P2-12: normal ESP32

| ID | Setup and execution | Fixture validity vs ESP PASS criterion |
| --- | --- | --- |
| P2-09 | Approved authoritative primary flash and boot, correct security/port, physical capture/logging | No fixture required; verified writes, correct boot/IP within shared30s, first ESP frames captured; setup error ->BLOCKED for later CoAP cases |
| P2-10 | Standard56830, one primary run | Fixture N/A; ESP20/20 exact correlated PONG, no panic/terminal error/cleanup warning, packet evidence |
| P2-11 | Fresh standard server and independent board reset/run | Fixture N/A; second20/20 with counters restarted; record changing ephemeral port |
| P2-12 | Analyze qualifying captures; optional approved pass proxy run with backend56831 | Correct v1 CON POST/path/token4/new request MID/text0/PING;2.04/PONG; no retries in qualifying healthy campaign. Typical2 datagrams; valid separate flow4 must be explicitly classified, not confused with retry |

## P2-13 through P2-18: reliability (primary)

| ID / mode | Fixture must demonstrate | ESP/overall PASS criterion |
| --- | --- | --- |
| P2-13 drop-first-request | First front request dropped before backend; later identical retry forwarded | Same MID/token/body retransmitted by library, logical attempt count unaffected; complete20/20 |
| P2-14 drop-first-response | First backend response dropped; repeated request/cache reply visible | Library retries same request; server application does not handle duplicate logical request twice;20/20. Distinguish backend datagram count from server Request logs |
| P2-15 duplicate-request | Identical first request forwarded again near250ms | Server handles logical request once, cached reply; ESP success once;20/20 |
| P2-16 duplicate-response | First response forwarded twice identical near250ms | ESP no duplicate success, no extra logical transaction;20/20 |
| P2-17 separate | Empty ACK, then CON2.04/PONG after250ms with new MID/same token | Empty ACK alone not success; ESP ACKs response MID;20/20. Capture ORIGINAL transaction20 response ACK before cleanup |
| P2-18 separate-duplicate | Same separate CON repeated at500ms | Transactions1..19: repeated CON ACKed without extra success,20/20. Transaction20: original response ACK required; duplicate arriving after completed shutdown outside active receive window is not required to be ACKed |

Use backend56831 for first four, synthetic56830 for separate modes. Fixture scheduling is not library retransmission. First retry nominal2..3s; record capture timestamps/tolerance. Duplicate-response totals must be counted on device-facing interface, excluding loopback duplicates. A host send log is not ESP receipt evidence.

## P2-19 through P2-25: negative cases (primary)

All use fresh synthetic fixture/no normal56830 server. Enforce60s readiness; do not terminate early during valid long observation. Firmware communication FAIL is expected; overall PASS depends on correct injection and rejection/bounded behavior.

| ID / mode | Fixture operation | ESP/overall PASS criterion |
| --- | --- | --- |
| P2-19 blackhole | Receives/logs requests, no response, no fixture timeout first | Initial+4 identical retransmissions, same MID/token; nominal62..93s exhaustion after first send; one logical attempt/zero successes, NACK and no next transaction. Unrelated early network failure is not retry-exhaustion PASS |
| P2-20 ack-only | Correct empty matching-MID ACK, then silence | No success; request retries cease; application reaches approximately100s ceiling;1/0 and bounded failure. Host180s watchdog is NOT this application deadline |
| P2-21 wrong-token | Correct MID/PONG/code but changed four-byte token | Never accept;1/0 bounded timeout/NACK/failure; do not require RST in response to ACK or guaranteed recovery from malformed ACK |
| P2-22 wrong-code; bad-payload FAIL/LF/OVERSIZE | Four independent runs:4.00,FAIL4,PONG+LF5,100X | Each matching invalid reply rejected, normally error6;1/0, no second transaction |
| P2-23 malformed | Truncated3-byte packet then valid reply after250ms | No crash/false success from bad packet; intended recovery20/20. Bounded failure is recorded FAIL/finding, not automatically accepted |
| P2-24 wrong-port | Correct reply from separately bound wrong port, retained in capture | Never accept wrong peer;1/0 bounded termination; record ICMP/socket effects independently |
| P2-25 rst | Empty matching-MID RST | Reset-related NACK;1/0, no next transaction, clean return |

If first request misses60s or host watchdog expires before intended result, fixture INVALID/overall BLOCKED, ESP observed result still recorded independently. A valid injected fault with wrong ESP behavior is FAIL. Unexpected fixture error cannot be hidden behind a device FAIL line.

## P2-26 through P2-28: resources/lifecycle

Use standard56830 server directly, no fault fixture. Require qualifying20/20, correct authoritative image and stable practical conditions. Do not mix negative runs into resource baselines.

| ID | Execution | PASS and measurement validity |
| --- | --- | --- |
| P2-26 primary | Two independent P2-PRIMARY-G3B runs; reuse P2-10/11 only if all identity/log/capture/conditions complete | Record B1/B2/B3/B5/B6, whole-run minimum, task counts; complete valid evidence and no measurement error. No invented numeric memory budget |
| P2-27 scoped | Flash P2-SCOPED-G3B by same safe procedure; two independent runs | Record before/after monitor, B3, B4 before stop, B5/B6 after stop, successful measurement status. No primary whole-run interpretation of scoped watermark |
| P2-28 lifecycle/tasks | Review qualifying primary/scoped logs and P2-17/18 final ACK evidence | Main allocation4096, task snapshots/no project task, no panic/stack overflow/cleanup warning, return from app_main. Task-count delta alone does not attribute ownership |

B1 before NVS/network/Wi-Fi; B2 after IPv4 before CoAP; B3 after CoAP setup before first request (scoped includes monitor); B4 scoped campaign-only per-region minima; B5 pre-CoAP cleanup after monitor stop; B6 main minimum free stack at result stage. Whole-run minimum only primary; not B4. Unreached checkpoints remain unavailable, not zero. Abort readings are not a completed campaign.

Monitor before-minus-after free heap is an observed start delta including allocator/concurrent SDK activity, not precise correction. B4 includes bookkeeping/logging; aggregate sums per-region minima, not necessarily simultaneous global free heap. Runtime allocation/stack effect remains pending actual G4. Derived main occupancy4096-B6 is observed history, not worst-case guarantee. Keep communication and measurement validity separate.

Historical Phase1 R1/R2 bytes: B1=329724/329724; B2=278212/278200; B5=277576/277576; B6=1748/1748; whole-run minima275460/274008. Missing historical clean-build B0, B3/B4 remain missing. Free-heap delta=CoAP free-UDP free; occupied delta has opposite sign. Do not fabricate missing comparisons, sum overlapping regions/heap/task stacks, attribute all firmware to CoAP or infer Beken resources. Section/build attribution remains separate from runtime evidence.

## P2-29: Beken portability assessment

Require actual Beken chip/SDK identifiers. Source/document assessment only: map used compiler/build, socket/I/O-wait, monotonic timing, allocator/alignment, random, task/callback and future Mbed TLS backend requirements. Record confirmed support, adaptation/gaps and verification limits. PASS requires a credible documented target-specific path; missing SDK/source identifiers ->BLOCKED target-specific assessment. Generic lwIP support is not proof of a working port. No Beken source changes/hardware or security implementation.

## Consolidated acceptance and cleanup

- All P2-01..29 have setup, expected/actual result, five separate reporting fields and evidence references; blocked/unexecuted items explicit.
- Capture prerequisites actually demonstrated; no claim from tool installation or mock validation alone. Physical/loopback observations separated; wrong-port/malformed traffic retained.
- Exactly one authoritative primary/scoped set selected by immutable member hashes; no silent rebuild/substitution; source/fixture/locks recorded.
- Fixture180s and first-request<=60s setup validity; firmware30s Wi-Fi/100s transaction deadlines distinguished; fixtures fresh per reset.
- Two independent20/20 healthy runs; standard correlation, loss recovery, duplicate suppression and separate-CON ACKs, including original final response, demonstrated.
- All negative cases produce no false success; malformed recovery assessed honestly; no hidden retries or combined intentional-loss metric.
- Primary/scoped resources valid or explicitly missing; historical limitations preserved; no invented budget or architecture/product conclusion.
- All test-owned processes stopped, ports released, logs/captures saved; source/branch/Phase1 user-file integrity rechecked. No commit/push/merge/tag implicit in testing.

G4 PROCEDURE STATUS: finalized for review; EXECUTION NOT AUTHORIZED. Stop for independent review of the watchdog amendment and documentation.

## Approved G4-A source correction - review addendum

This addendum supersedes historical preparation-only status text above for the current source correction. The accepted execution baseline is f7d0f1682d835611f61b658ff2e60b4607ac704d. G4-A was subsequently executed with P2-02..05 PASS, P2-01 BLOCKED, P2-06 FAIL, P2-07/P2-08 BLOCKED. Preserve those records and all four passing results; do not rebuild firmware because host networking failed.

Current user authorization permits only two cloud.js options and supporting static/mock validation: reuseAddr=false and sendAcksForNonConfirmablePackets=true. The first disables package-default address sharing; the second permits piggyback ACK sends through the inspected coap1.6.0 gate. Neither is yet a real-network PASS. Source is uncommitted pending independent review; record a separately reviewed correction identity before future execution. No P2-06..08 retest, capture setup, driver/firewall/network change, device access, commit/push or G4-B/C is authorized by this addendum.

After separate source review and execution authorization: record command, PID, creation time and endpoint ownership for every server/probe; verify required ports free before starting. P2-06 must show first server listening, second failing bind/nonzero exit, first remaining usable; retain actual Windows error code. Terminate/wait for the second test-owned process and confirm only intended owner before P2-07. Verify working loopback capture first; no probe PASS without saved/reopened packet evidence. Run valid then path/method/payload/format as fresh sequential approved probes, require expected2.04/4.04/4.05/4.00/4.00 with correct correlation/content, and preserve first-server availability. No extra modes or test IDs.

Do not impose the previous wrappers'10s/12s external kill deadlines: retain unchanged180s fixture watchdog, distinguish readiness from response timeout and record interrupted attempts. The60s first-device-request rule is unchanged and applies to later ESP32 fault runs, not these host probes. Keep fixture validity, ESP result (N/A here), overall outcome, actual packet evidence and runtime measurement validity (N/A here) separate. Firmware authoritative sets and existing build evidence remain unchanged. Stop for source review; request separate authorization for real network retesting only after corrected source is independently reviewed.
