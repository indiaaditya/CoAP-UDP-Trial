# Phase 2 G3-A source/build evidence — 2026-11-15

Status: G3-A was committed and pushed as aad119d4a980b37e510940e37a85b835a1630717 and independently accepted at source/build-review level. G3-B accepted at c146cb2353feb5cc467f85968fb9f6546fb1f607; G3-C fixture implementation authorized, G4 remains unauthorized. No hardware CoAP feasibility result, packet exchange, loss test or runtime resource reading is claimed.

## Git/worktree preservation

New worktree D:/proff/clients/Welmade/CoAP-UDP-Trial-phase2, branch phase2-coap-baseline, approved source baseline ac1fa50d6731fd3f607789862fa28fadd8bf84e4. Created from that exact baseline with git worktree add -b. Initially clean. G3-A implementation is committed/pushed as aad119d4a980b37e510940e37a85b835a1630717; G3-B is committed/pushed and accepted as c146cb2353feb5cc467f85968fb9f6546fb1f607; current G3-C work is uncommitted with no commit/push authorization.

Original D:/proff/clients/Welmade/CoAP-UDP-Trial remains phase1-udp-baseline at the same hash, with modified project-doc/Notings.xlsx and untracked project-doc/plantUML/FlowChart/Phase2 ESP32.png. Before/after SHA256: workbook dedd61751b37f14d64d01be8c45d81d555e8ec092b7972b5ef7dbb55e28ff6f8; PNG 213a2495b22a5588ba19fcd0b7dd3156e42fd2eefb37f46740c2b282955f400d. Workbook was open; read-sharing hash used without modifying it. No stash/reset/clean/stage/commit/checkout in original directory; no existing branch reference modified. Worktree registration necessarily uses shared Git metadata. New worktree contains committed workbook, not original uncommitted workbook/PNG.

## Build environment and commands

Windows PowerShell; Node v25.9.0/npm11.12.1. ESP-IDF 5.5.4 at C:/Espressif/frameworks/esp-idf-v5.5.4, exact commit735507283d5b2f9fb363a1901172dbd9e847945d. SDK tracked diff empty on inspection. GCC14.2.0 esp-14.2.0_20260121, Python3.11.2. Existing installation only. Process-local safe.directory entries used for verified repositories; no global Git configuration edits.

Firmware command, from phase2-coap-baseline/src/esp32-coap-client:

```powershell
$env:IDF_TOOLS_PATH = 'C:\Espressif'
$env:PATH = 'C:\Espressif\python_env\idf5.5_py3.11_env\Scripts;' + $env:PATH
. C:\Espressif\frameworks\esp-idf-v5.5.4\export.ps1
idf.py --no-ccache -DIDF_TARGET=esp32s3 build
```

Build directory did not exist before this invocation; generated configuration came from the archived committed baseline snapshot plus approved CoAP settings. Configure resolved espressif/coap4.3.5~8 and idf5.5.4. Clean build completed all1106 main actions plus123 bootloader actions, exit0; application906608 bytes (0xdd570), factory partition1048576 bytes, remaining141968 (0x22a90), tool-rounded14%. Bootloader22272 bytes (0x5700). No compiler warning/error found in build logs, including project main.c and managed component. Export's unused-old-tools notice is informational; no tools removed. 'USING O3' relates to a separately configured component; main compile command remains -Og and baseline global debug optimization unchanged.

The app version was automatically derived by ESP-IDF as phase1-udp-baseline-v1.0-dirty from existing repository metadata; it does not indicate firmware was flashed or accepted as Phase1. No tag was created. Source/config/artifact hashes below establish identity. No second build or size-attribution campaign was run; full resource work remains G3-B.

Node command from phase2-coap-baseline/src:

```powershell
$env:NODE_OPTIONS = '--use-system-ca'
npm install --ignore-scripts --no-audit --no-fund
node --check cloud.js
npm ls --all
```

Initial installation without system-CA option failed UNABLE_TO_VERIFY_LEAF_SIGNATURE. Retried with OS trust store, retaining TLS verification: installed25 packages, exit0. Syntax check exit0. Module-load smoke check reported coap=1.6.0 createServer=function; no server was started/bound. No audit/install scripts/new frameworks. G3-C direct test dependency not installed separately (coap-packet exists only as production transitive dependency).

## Actual artifact identities

| Phase-relative path | Bytes | SHA256 |
| --- | ---: | --- |
| src/cloud.js | 2631 | 486ee7ba8cc4fdc63501ae32be48b443a11aab7d8dfc8a7ad127586ef9bbce0f |
| src/package-lock.json | 10714 | 9a78ae2c93cea54ed64f6f4559119b22db6bc98eecd328effa5fbfd2827f4c3c |
| src/esp32-coap-client/main/main.c | 23820 | a10922991674433488033d06e4de094566a7c832ce8e902d5c97124cf21145da |
| src/esp32-coap-client/sdkconfig | 76553 | fdaadd3f880ff8a6f2c5e0be08f01113a134db6272bd63879d13dbd06764e3e3 |
| src/esp32-coap-client/build/esp32_coap_client.bin | 906608 | c33d71b3f25865515c401873941dff54450011a351ab63dc3cc378535849924a |
| src/esp32-coap-client/build/esp32_coap_client.elf | 10069552 | 61a488efcd22b7c587da348fd99c399305d27aa551bed1bb5bbf0bd54d98f21b |
| src/esp32-coap-client/build/esp32_coap_client.map | 8837535 | 880e8d6550f71271f8bab1cfd2443e42da01c00f55015d42de398f719d3d6720 |
| src/esp32-coap-client/build/bootloader/bootloader.bin | 22272 | 38030af8b3a66e5d4f54918705cadac547d04fbe7276f4befad55c141855ee0d |

Binaries/ELF/map/config remain local ignored artifacts. Credentials were copied privately to the new worktree only; their values never printed/recorded. Exact bitwise reproduction also depends on private inputs/build metadata.

## Dependency resolution

Official component hash d5d145e6b7755be1d4d9066fbced4eef2348e1d6cf8bf842e4f16e2bcbfcaf41. SDK and library revisions/requirements in dependencies.lock. Upstream component sources unmodified. Component requires lwip/mbedtls; no custom adapter.

| npm package | Resolved version |
| --- | --- |
| @types/node | 26.6.6 |
| @types/readable-stream | 4.0.25 |
| abort-controller | 3.0.0 |
| base64-js | 1.5.1 |
| bl | 6.1.6 |
| buffer | 6.0.3 |
| capitalize | 2.0.4 |
| cbor | 9.0.2 |
| coap | 1.6.0 |
| coap-oscore | 2.2.3 |
| coap-packet | 1.1.1 |
| debug | 4.4.3 |
| event-target-shim | 5.0.1 |
| events | 3.3.0 |
| fastseries | 2.0.0 |
| ieee754 | 1.2.1 |
| inherits | 2.0.4 |
| lru-cache | 11.5.3 |
| ms | 2.1.3 |
| nofilter | 3.1.0 |
| process | 0.11.10 |
| readable-stream | 4.8.0 |
| safe-buffer | 5.2.1 |
| string_decoder | 1.3.0 |
| undici-types | 8.9.0 |

coap-oscore and cbor installed by coap but no OSCORE context/security configured.

## Effective configuration comparison

Compared all normalized CONFIG assignments/unset symbols against committed Phase1 effective snapshot. No pre-existing setting changed. Added only CONFIG_COAP_* symbols: CLIENT_SUPPORT=y, INTERNAL_MEM_ALLOC=y, LOG_DEFAULT_LEVEL=0; ASYNC_SUPPORT, CUSTOM_MEM_ALLOC, DEFAULT_MEM_ALLOC, DEBUGGING, MBEDTLS_DEBUG, MBEDTLS_PKI, MBEDTLS_PSK, OBSERVE_PERSIST, OSCORE_SUPPORT, Q_BLOCK, SERVER_SUPPORT, TCP_SUPPORT, THREAD_SAFE, WEBSOCKETS=n. PROXY_SUPPORT is gated absent/disabled by server off; no proxy compilation selected. Existing Mbed TLS TLS-enabled setting preserved, no TLS/DTLS session. No G3-B Kconfig option yet.

## Verification and limitations

Static checks: exactly5 C custom function definitions and4 named Node callbacks; original Wi-Fi callback/setup/cleanup retained; coap_send ownership correct; main waits for coap_io_process return before final-success cleanup; response pointer lifetime limited to callback; config/ignore/secret exclusions inspected; whitespace check clean. Main C source timestamp precedes its compiled object. No upstream modification. Independent G3-A source/build review accepted commit aad119d4a980b37e510940e37a85b835a1630717.

G3-A implements all approved communication functions and essential logs. Full measurement storage/task counts/scoped monitor builds were deferred at G3-A and are implemented under the subsequent G3-B authorization. Node fault fixture/dev dependency were absent at G3-A/B and are now implemented under G3-C. Runtime PING/PONG, reliability, final ACK on wire, stack sufficiency, cleanup behavior, Node error branches and Beken compatibility are Not Verified. No G4 functional tests, network server or device access. Compilation alone is not demonstrated CoAP feasibility.

Design deviations: none requiring altered functionality, functions, dependencies or feature settings. Staged deferrals are explicitly authorized G3-A boundaries. Full G2 and clarifications retained through G2_DESIGN.md and actual registries.

Final static verification: exact Wi-Fi callback and guarded cleanup byte-for-byte unchanged as text from committed Phase1 source; Wi-Fi initialization segment unchanged except B2 label and equivalent capability constant. Main compile command has -Og. Source timestamp precedes compiled object. All22 candidate source/config/record files scanned for the two private literal values without exposing them; no matches. No fixture/scoped Kconfig present. These are source/build checks, not runtime tests.


## G3-B separate clean builds and size analysis

G3-A was accepted at aad119d4a980b37e510940e37a85b835a1630717. G3-B changes were subsequently committed/pushed and accepted as c146cb2353feb5cc467f85968fb9f6546fb1f607. Current G3-C changes remain uncommitted for review. Only app_main measurement operations/state plus the approved Kconfig/defaults arrangement changed; other four custom functions, whole campaign/pacing loop and CoAP/Wi-Fi teardown are text-identical to the accepted source. Node server/manifests/locks unchanged. No application task, abstraction, upstream change or security/protocol expansion.

Both output directories were verified absent before the following invocation, then built from scratch. Existing IDF export and process-local safe.directory settings as in G3-A. No build deletion/Phase1 rebuild. Commands from phase2-coap-baseline/src/esp32-coap-client:

```powershell
idf.py --no-ccache -B build-primary -DIDF_TARGET=esp32s3 -DSDKCONFIG=build-primary/sdkconfig '-DSDKCONFIG_DEFAULTS=sdkconfig.defaults' build
idf.py --no-ccache -B build-scoped -DIDF_TARGET=esp32s3 -DSDKCONFIG=build-scoped/sdkconfig '-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.scoped.defaults' build
```

Both builds completed successfully (exit0). Primary907040 bytes, scoped907984, bootloaders22272 each, both fit existing1MiB app partition. No compiler warning/error in saved logs. IDF export's unused old tools notice remains informational. G3-A build/ binary hash remains c33d71b3f25865515c401873941dff54450011a351ab63dc3cc378535849924a. Variant configs contain the same baseline/global CoAP settings; their only normalized symbol difference is CONFIG_TRIAL_SCOPED_HEAP_MONITOR n ->y. Primary versus G3-A adds only that new n symbol. Dependency locks unchanged. Main allocation4096, debug -Og, PSRAM off, SDK/compiler unchanged. Metadata version is still automatically derived from existing Git tags/dirty source; source/config/image ledger is authoritative.

Size reports were generated without relinking using the installed parser. For each of build-primary and build-scoped:

```text
python -m esp_idf_size.ng --format json2 <dir>/esp32_coap_client.map -o <dir>/summary.json
python -m esp_idf_size.ng --format json2 --archives <dir>/esp32_coap_client.map -o <dir>/archives.json
python -m esp_idf_size.ng --format json2 --files <dir>/esp32_coap_client.map -o <dir>/files.json
```

Retained G3-A and identity-verified Phase1 maps were read with the same ng parser for section counts and legacy --format json --archives/--files for continuity with the committed Phase1 attribution. Historical read results were saved only inside Phase2 ignored build/, never in original Phase1 directory. Retained map/ELF hashes match Phase1 committed provenance. No new historical runtime or clean-build provenance is invented.

RESOURCE_BASELINE.md contains complete section/component tables, the exact148688-byte historical binary reconciliation, merged-string attribution caveat, Mbed TLS/backend retained code, instrumentation overhead and artifact/source identities. It separates58222 additional loaded bytes in Mbed TLS/crypto/X509 archives from4769 backend bytes already inside the CoAP component; no double counting. Source/map analysis also identifies generic backend-context initialization even for UDP; no secure session or certificate transaction is configured/performed.

G3-B source instrumentation: stored B1/B2/B3/B5/B6 and primary whole-run minima; before/after task counts; scoped monitor start before B3, B4 read before stop, stop before CoAP cleanup; monitor bookkeeping before/after snapshots and separate measurement-error status. No whole-run minimum is presented by scoped build. Unreached/abort checkpoints retain explicit scope; no false zero values. Monitor allocation failure can assert under SDK implementation; runtime overhead cannot be fully corrected using a simple heap subtraction.

All runtime heap/stack/task/network values remain Not Measured. P2-03/04 build and P2-05 configuration/static evidence only; no P2-26/27 runtime campaign, no G3-C fixture, no G4 hardware or network access. No speculative optimization. Await independent G3-B source/build review and separate commit/push permission.

Historical G3-B verification before its later commit/push: installed esp-idf-size1.7.1 used for both parser interfaces; five custom C functions remain. Source/configuration lock comparison shows only the new measurement flag. Neither Node source nor dependency lockfiles changed. Source predates compiled main object in both variants; -Og retained. No warning/error matched in either saved build log. All source/record candidates checked for private credential literals; no generated/private paths intended for tracking. Original Phase1 branch/status/workbook/PNG hashes remain unchanged. No commit/push performed.


## G3-C host fixture implementation and non-network validation

G3-B accepted source/build commit c146cb2353feb5cc467f85968fb9f6546fb1f607. G3-C changes are local/uncommitted; no push authorization. G4 hardware/network testing is not authorized. Standard server and all ESP32 source/instrumentation/configuration/dependency locks remain byte-identical to accepted G3-B; no firmware rebuild or device access.

Historical G3-C validation at accepted commit7dbe6f2: implemented test/coap_fixture.js with exactly11 approved named functions, sixteen approved modes, optional bad-payload/probe CASE,256-record cap,120s watchdog, peer validation, named send/error/listen/signal callbacks and idempotent cleanup. No arrow/anonymous callback implementation functions. Scoped synthetic packets are not a general CoAP server/framework. Standard server has no fault-injection flags or modifications.

Dependency update, from src:

```text
npm install --package-lock-only --ignore-scripts --no-audit --no-fund --offline
node --check ../test/coap_fixture.js
node --check cloud.js
npm ls coap coap-packet
```

All commands exit0. coap1.6.0 unchanged; direct test-only coap-packet1.1.1 shares the existing production transitive installation. Lock diff changes root devDependencies only; all25 resolved versions/integrities unchanged. No network install, new package/framework or runtime source changes.

Validation harness executed from stdin using node:vm, mocked dgram socket EventEmitters, deterministic in-memory timers, Buffer and the actual installed public coap-packet1.1.1 codec. No real socket was opened, no packet transmitted, no sleep/real120s timer elapsed, and no server/ESP32 network process was started. The ephemeral validation harness is not another repository implementation file. The source under review retains only the approved11 fixture functions.

Initial fixture harness result:48/49 passed. The sole failure was an erroneous expected request size23 in the validation assertion, inherited from the earlier design arithmetic. Actual codec output was24. Targeted corrected assertion passed (request24,response14), with no fixture/protocol/source change. The ledger retains the initial failure and its resolution. Resolved fixture checks49/49; unchanged-server mocked application callback checks14/14. This is not a claim of live interoperability, OS UDP timing, real package server reliability or ESP32 acceptance.

| Fixture check (resolved; initial size assertion correction noted above) | Result |
| --- | --- |
| public codec request/response lengths | PASS |
| proxy pass | PASS |
| proxy drop-first-request | PASS |
| proxy drop-first-response | PASS |
| proxy duplicate-request | PASS |
| proxy duplicate-response | PASS |
| proxy preserves client control ACK forwarding | PASS |
| proxy ignores unexpected backend/front peers | PASS |
| blackhole suppresses all five identical requests | PASS |
| ACK-only is empty four-byte matching ACK | PASS |
| separate response timing/correlation/ACK handling | PASS |
| separate-duplicate response timing/correlation/ACK handling | PASS |
| separate MID wrap and repeated-request suppression | PASS |
| separate supports 20 distinct sequential transactions within capture bound | PASS |
| wrong-token modifies only correlation token | PASS |
| wrong-code is matching-token 4.00 | PASS |
| bad-payload FAIL | PASS |
| bad-payload LF | PASS |
| bad-payload OVERSIZE | PASS |
| malformed truncated header precedes valid response at 250ms | PASS |
| wrong-port uses distinct bound socket | PASS |
| RST is empty matching-MID reset | PASS |
| probe valid request/response | PASS |
| probe path request/response | PASS |
| probe method request/response | PASS |
| probe payload request/response | PASS |
| probe format request/response | PASS |
| probe empty ACK does not complete; separate response ACK send precedes finish | PASS |
| probe rejects bad token | PASS |
| probe rejects bad code | PASS |
| probe rejects bad payload | PASS |
| probe rejects bad format | PASS |
| probe rejects bad mid | PASS |
| invalid CLI [null] | PASS |
| invalid CLI ["unknown"] | PASS |
| invalid CLI ["pass","extra"] | PASS |
| invalid CLI ["bad-payload","bad"] | PASS |
| invalid CLI ["probe","bad"] | PASS |
| watchdog stops at 120s, not before | PASS |
| pending delayed sends cancelled on shutdown and finish idempotent | PASS |
| capture limit bounded | PASS |
| unexpected malformed client input fails safely | PASS |
| invalid synthetic request fails safely | PASS |
| socket error closes all sockets once | PASS |
| synchronous send error stops safely | PASS |
| asynchronous send error stops safely | PASS |
| bind throw stops startup safely | PASS |
| close error records failure | PASS |
| fixture function allowlist | PASS |

| Unchanged server application callback (mock package; no network) | Result |
| --- | --- |
| valid | PASS |
| path | PASS |
| method | PASS |
| payload | PASS |
| missing-format | PASS |
| wrong-format | PASS |
| excluded-Observe | PASS |
| excluded-Block1 | PASS |
| excluded-Block2 | PASS |
| excluded-Q-Block1 | PASS |
| excluded-Q-Block2 | PASS |
| excluded-Proxy-Uri | PASS |
| excluded-Proxy-Scheme | PASS |
| excluded-OSCORE | PASS |

Validation source/evidence SHA256:

- fixture: a3aa592c62d97095142939540f23954cee486afe9a755bd1420eecdf87e51048
- fixture validation JSON: 359bdb15aae935ce8d6a273feea2d587d1aa0384442b788d9200af8bc5973c30
- server callback validation JSON: eeef85ac2d996ae31f82af3ce97e1bfa11656cf993b0b6f371682426152c8e89

Local reports contain test names/status and the corrected arithmetic provenance, not packet captures. Excluded from Git by existing build-directory rule. Existing primary/scoped firmware artifacts preserved.

Unexecuted G4 evidence: actual server/proxy socket startup and occupied-port behavior, real UDP source identity and delay accuracy, package duplicate cache/retry behavior, ESP32 PING/PONG/retransmission/ACK/RST/malformed-response handling, final20th separate-CON ACK on wire, packet capture, runtime heap/stack/task measurements and Beken target-specific assessment. All remain pending explicit authorization. No G4 result is inferred from mock checks.

Final G3-C static check: all15 changed/new files remain confined to approved fixture/manifests/records. Eleven exact fixture functions, no extra implementation callbacks. Whole tracked ESP32 tree, standard cloud.js and both existing firmware build variants unchanged. All installed dependency records (versions, integrities, source locations) match accepted G3-B; only root exact devDependency added. Whitespace and syntax checks pass. No private credential literal found in candidate files. Original Phase1 branch/status/workbook/PNG hashes match start of this G3-C task. No commit/push or real socket/network/device operation.


## Approved watchdog amendment - host-only validation and finalized G4 procedure

Accepted G3-C source commit7dbe6f2298fbdcbeee34669c097ca55648b22f71; current amendment uncommitted for review. Exact fixture diff is one constant: FIXTURE_TIMEOUT_MS120000 ->180000. Eleven functions, sixteen modes, all other timing constants, dependencies and socket behavior unchanged. ESP32 communication/instrumentation and cloud.js untouched. Historical G3-C49-check results above used120s and remain historical; the CURRENT watchdog expectation and tests use180s.

Syntax: node --check phase2-coap-baseline/test/coap_fixture.js and node --check phase2-coap-baseline/src/cloud.js both exit0. Applicable validation executed from stdin using node:vm/EventEmitter mocked dgram, deterministic virtual clock/timers and actual public coap-packet1.1.1. No real sockets, network, wall-clock wait or hardware.27/27 checks PASS:

| Applicable amendment check | Result |
| --- | --- |
| exact one-line fixture source amendment | PASS |
| exact eleven functions and sixteen modes | PASS |
| pass startup uses180000ms and operator cleanup | PASS |
| drop-first-request startup uses180000ms and operator cleanup | PASS |
| drop-first-response startup uses180000ms and operator cleanup | PASS |
| duplicate-request startup uses180000ms and operator cleanup | PASS |
| duplicate-response startup uses180000ms and operator cleanup | PASS |
| blackhole startup uses180000ms and operator cleanup | PASS |
| separate startup uses180000ms and operator cleanup | PASS |
| separate-duplicate startup uses180000ms and operator cleanup | PASS |
| ack-only startup uses180000ms and operator cleanup | PASS |
| wrong-token startup uses180000ms and operator cleanup | PASS |
| wrong-code startup uses180000ms and operator cleanup | PASS |
| bad-payload startup uses180000ms and operator cleanup | PASS |
| malformed startup uses180000ms and operator cleanup | PASS |
| wrong-port startup uses180000ms and operator cleanup | PASS |
| rst startup uses180000ms and operator cleanup | PASS |
| probe startup uses180000ms and operator cleanup | PASS |
| alive at old120s boundary; timeout only at180s | PASS |
| latest-ready blackhole request has93s retry observation window | PASS |
| latest-ready ACK-only request has100s application observation window | PASS |
| separate/duplicate250ms500ms timings and packet identities unchanged | PASS |
| shutdown cancels pending stimulus and watchdog idempotently | PASS |
| capture256 bound unaffected | PASS |
| invalid mode still opens no sockets and fails | PASS |
| socket error path still closes owned sockets | PASS |
| request24 response14 and emptyACK4 unchanged | PASS |

Fixture SHA256 b2eb53c73077cacba6974ee241922745898dad95069717f581f2d4a3ec65ebe4. Local ignored validation report src/esp32-coap-client/build/watchdog-amendment-validation.json SHA256 b884bbf413ee82b2fc93e357b1b66fc45cb40d511a88284d76bd317483a38bba; report records exact executed source hash, method, Node version and all check results. No persistent validation implementation file added.

Long-run models inject first request at virtual60s: blackhole remains active through virtual153s (93s observation), ACK-only remains active through160s (100s observation) and179.999s;180s callback fails/cleans up. These test FIXTURE availability only, not actual ESP32 retry/deadline correctness. Separate/duplicate stimuli remain250ms/500ms.60s readiness is explicitly operator-enforced, not a source timer or callback change.

Manual procedure finalized without expanding P2-01..29. Capture-tool availability and successful physical/loopback capture are required prerequisites, presently NOT VERIFIED. No installation/capture attempted. Read-only artifact hash checks matched both retained G3-B sets; MANUAL_TEST_PROCEDURE.md designates P2-PRIMARY-G3B and P2-SCOPED-G3B with app/ELF/map/bootloader/partition/config hashes. This is immutable selection by identity, not a rebuild, hardware verification or filesystem-permission change. Fresh future verification builds cannot silently replace those selected sets.

Each future run separately reports fixture validity, ESP communication result, overall result, packet evidence and measurement validity. Late first request/setup watchdog ->fixture INVALID/overall BLOCKED, not ESP protocol failure. Valid fault with intended ESP rejection can be overall PASS while ESP communication FAIL. No G4 test, flash, monitor, network process, branch operation, commit or push was executed. Original Phase1 directory/reference/user files preserved. Stop for independent review.

## G4-A historical execution and approved source-only correction

Accepted baseline f7d0f1682d835611f61b658ff2e60b4607ac704d; current correction uncommitted for independent review. Prior real G4-A evidence remains immutable at D:/proff/clients/Welmade/CoAP-G4-Evidence/20261115-172840-IST. All 58 evidence-manifest entries matched during read-only investigation. P2-02, P2-03, P2-04 and P2-05 remain PASS; no clean-build rerun. P2-01 capture readiness BLOCKED; P2-06 historical FAIL; P2-07/P2-08 BLOCKED. No correction retrospectively changes these results.

Historical P2-06: PIDs 23044 and 6316 both reported 0.0.0.0:56830 listening. The installed coap 1.6.0 defaults reuseAddr=true and passes it to Node dgram.createSocket, explaining shared binds. Wrapper terminates/waits for PID 6316 before initial probe; first server logs peer 127.0.0.1:53904. Authoritative fresh-server attempt is associated with PID 8612; valid probe 127.0.0.1:65288 sends 24-byte CON POST/MID42/token8000002a. Server logs request receipt and 2.04/PONG intent, fixture logs no incoming response and expires after approximately 180.135 seconds. Path probe 127.0.0.1:53763 was interrupted; method/payload/format not executed. No captured packets exist. Dedicated logs/process records identify the intended server, but no comprehensive endpoint snapshot exists at every probe instant.

High-confidence P2-07 explanation from installed code: outgoing_message.js constructs ack=true/confirmable=false piggyback replies; server.js _finishSendResponse gates sender.send on sendAcksForNonConfirmablePackets===true or outgoing packet.confirmable. Old false setting suppresses the immediate ACK response. This static branch explains absence of send errors/fixture ingress without claiming runtime branch or network transmission proof. UDP sends, when invoked, complete asynchronously; res.end() and application log are not delivery evidence.

Approved exact source correction: add reuseAddr=false; change sendAcksForNonConfirmablePackets=false -> true. No other server source differences; all four callback bodies/registration tail unchanged. coap pinned/locked/installed at1.6.0. ESP32, fixture, dependency manifests and lockfiles unchanged; all12 authoritative firmware member hashes match. The option name does not precisely express its inspected outgoing-packet gate; true also changes gating outside the current CON-only workload, without changing packet flags or adding a test scope.

Current validation: node --check phase2-coap-baseline/src/cloud.js exits0. VM mocks only coap.createServer/listen, req/res and process/console; no actual coap runtime loaded, sockets, network, timers, device or firmware build. Original14 request cases plus11 option/endpoint/error cases pass25/25:

| Mock check | Result |
| --- | --- |
| valid | PASS |
| path | PASS |
| method | PASS |
| payload | PASS |
| missing-format | PASS |
| wrong-format | PASS |
| excluded-Observe | PASS |
| excluded-Block1 | PASS |
| excluded-Block2 | PASS |
| excluded-Q-Block1 | PASS |
| excluded-Q-Block2 | PASS |
| excluded-Proxy-Uri | PASS |
| excluded-Proxy-Scheme | PASS |
| excluded-OSCORE | PASS |
| exact approved SERVER_OPTIONS passed to createServer | PASS |
| default endpoint and callback registrations | PASS |
| backend override remains56831 | PASS |
| invalid port rejected-0 | PASS |
| invalid port rejected-65536 | PASS |
| invalid port rejected-NaN | PASS |
| invalid port rejected-56830.5 | PASS |
| listening success logs endpoint | PASS |
| listening error delegates failure and closes once | PASS |
| repeated server errors close once | PASS |
| response error logs without close or invented success | PASS |

Corrected cloud.js SHA256 cfe6a3c6aaa0f949973cb786bf8a9d54a1c6b73257936b72adec2ab9ad0da5c3. External reproducible harness and JSON report: D:/proff/clients/Welmade/CoAP-G4-Evidence/20261115-220757-IST-server-options/validate-server-options.js and server-mock-validation.json. static-validation.json records exact two-option source equivalence, four unchanged callback bodies, version preservation and12 artifact matches; evidence-sha256.json inventories local evidence. No secrets intentionally copied into evidence. Mocked EADDRINUSE/EACCES and response-error checks verify callback policy only, not actual OS errors or package send/delivery.

Remaining validation requires independent source review followed by separately authorized P2-06 controlled double-bind and P2-07/08 real loopback probes with working packet capture, process/endpoint snapshots and complete logs. Physical passive capture readiness also remains BLOCKED until verified. All five reporting fields remain separate; original failures are historical evidence. No current real retest, software/driver/firewall/config change, hardware access, commit/push. Stop for review.
