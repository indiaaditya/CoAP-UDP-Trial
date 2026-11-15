# Phase 2 G3-A source/build evidence — 2026-11-15

Status: G3-A was committed and pushed as aad119d4a980b37e510940e37a85b835a1630717 and independently accepted at source/build-review level. G3-B instrumentation/build work is authorized; G3-C and G4 remain unauthorized. No hardware CoAP feasibility result, packet exchange, loss test or runtime resource reading is claimed.

## Git/worktree preservation

New worktree D:/proff/clients/Welmade/CoAP-UDP-Trial-phase2, branch phase2-coap-baseline, approved source baseline ac1fa50d6731fd3f607789862fa28fadd8bf84e4. Created from that exact baseline with git worktree add -b. Initially clean. G3-A implementation is committed/pushed as aad119d4a980b37e510940e37a85b835a1630717; current G3-B work is uncommitted and has no commit/push authorization.

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

G3-A implements all approved communication functions and essential logs. Full measurement storage/task counts/scoped monitor builds were deferred at G3-A and are implemented under the subsequent G3-B authorization. Node fault fixture/dev dependency absent. Runtime PING/PONG, reliability, final ACK on wire, stack sufficiency, cleanup behavior, Node error branches and Beken compatibility are Not Verified. No G4 functional tests, network server or device access. Compilation alone is not demonstrated CoAP feasibility.

Design deviations: none requiring altered functionality, functions, dependencies or feature settings. Staged deferrals are explicitly authorized G3-A boundaries. Full G2 and clarifications retained through G2_DESIGN.md and actual registries.

Final static verification: exact Wi-Fi callback and guarded cleanup byte-for-byte unchanged as text from committed Phase1 source; Wi-Fi initialization segment unchanged except B2 label and equivalent capability constant. Main compile command has -Og. Source timestamp precedes compiled object. All22 candidate source/config/record files scanned for the two private literal values without exposing them; no matches. No fixture/scoped Kconfig present. These are source/build checks, not runtime tests.


## G3-B separate clean builds and size analysis

G3-A was accepted at aad119d4a980b37e510940e37a85b835a1630717. Current source changes remain uncommitted/unpushed for review. Only app_main measurement operations/state plus the approved Kconfig/defaults arrangement changed; other four custom functions, whole campaign/pacing loop and CoAP/Wi-Fi teardown are text-identical to the accepted source. Node server/manifests/locks unchanged. No application task, abstraction, upstream change or security/protocol expansion.

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

Final G3-B verification: installed esp-idf-size1.7.1 used for both parser interfaces; five custom C functions remain. Source/configuration lock comparison shows only the new measurement flag. Neither Node source nor dependency lockfiles changed. Source predates compiled main object in both variants; -Og retained. No warning/error matched in either saved build log. All source/record candidates checked for private credential literals; no generated/private paths intended for tracking. Original Phase1 branch/status/workbook/PNG hashes remain unchanged. No commit/push performed.
