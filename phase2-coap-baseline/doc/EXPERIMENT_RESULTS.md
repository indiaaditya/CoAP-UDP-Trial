# Phase 2 G3-A source/build evidence — 2026-11-15

Status: implementation candidate ready for independent source review. G3-B/C and G4 not authorized. No hardware CoAP feasibility result, packet exchange, loss test or runtime resource reading is claimed.

## Git/worktree preservation

New worktree D:/proff/clients/Welmade/CoAP-UDP-Trial-phase2, branch phase2-coap-baseline, HEAD ac1fa50d6731fd3f607789862fa28fadd8bf84e4. Created from that exact approved baseline with git worktree add -b. Initially clean. Implementation changes are uncommitted in this worktree only.

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

Static checks: exactly5 C custom function definitions and4 named Node callbacks; original Wi-Fi callback/setup/cleanup retained; coap_send ownership correct; main waits for coap_io_process return before final-success cleanup; response pointer lifetime limited to callback; config/ignore/secret exclusions inspected; whitespace check clean. Main C source timestamp precedes its compiled object. No upstream modification. Source review by the user is still pending.

G3-A implements all approved communication functions and essential logs. Full measurement storage/task counts/scoped monitor builds are intentionally deferred. Node fault fixture/dev dependency absent. Runtime PING/PONG, reliability, final ACK on wire, stack sufficiency, cleanup behavior, Node error branches and Beken compatibility are Not Verified. No G4 functional tests, network server or device access. Compilation alone is not demonstrated CoAP feasibility.

Design deviations: none requiring altered functionality, functions, dependencies or feature settings. Staged deferrals are explicitly authorized G3-A boundaries. Full G2 and clarifications retained through G2_DESIGN.md and actual registries.

Final static verification: exact Wi-Fi callback and guarded cleanup byte-for-byte unchanged as text from committed Phase1 source; Wi-Fi initialization segment unchanged except B2 label and equivalent capability constant. Main compile command has -Og. Source timestamp precedes compiled object. All22 candidate source/config/record files scanned for the two private literal values without exposing them; no matches. No fixture/scoped Kconfig present. These are source/build checks, not runtime tests.
