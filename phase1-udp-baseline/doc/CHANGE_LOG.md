# Change log

## 2026-11-10 — Phase 1, Step 1

Created branch phase1-udp-baseline from main at af4b50ef48305e17bdd77d4c899f0dbec1493cb5 after verifying no same-name local or remote branch. README.md remains unchanged.

Added phase1-udp-baseline/src/cloud.js for the approved dependency-free UDP baseline, all six phase documentation files, the three approved project-doc files, and empty test/.gitkeep and rls/.gitkeep within the phase directory. File purposes are recorded in FILE_REGISTRY.md.

Implemented exact PING/PONG, sender-directed replies, packet logging and basic socket/send errors to establish the request/response baseline. No future-phase functionality, dependencies or build artifacts introduced.

Validation: node --check passed. T01-T06 and T08 passed on localhost: 20 sequential UDP request/response exchanges plus one liveness request, 21/21 responses; five nonmatching packets received no response. T07 was attempted but injected Ctrl+C did not stop the tool-managed process; normal interactive termination was unverified in that automated run. Explicit cleanup released the port. Detailed evidence is in EXPERIMENT_RESULTS.md.

No merge or milestone tag created. Step 2 not started.

## Step 1 review update

The user reviewed cloud.js and found it acceptable; no source changes requested. T07 passed by user-reported manual Ctrl+C verification. This supplements the original automated attempt without replacing its evidence. The manual test date, runtime and platform were not supplied. Updated EXPERIMENT_RESULTS.md, this log, project-doc/PHASE_ROADMAP.md and project-doc/EXPERIMENT_COMPARISON.md to reflect the review and use UDP request/response exchange terminology. No source changes or additional automated tests were required. Step 2 and milestone tagging remain unauthorized.

## Project-wide development standard

Added project-doc/CODEX_DEVELOPMENT_GATES.md from the user-supplied standard and registered it in FILE_REGISTRY.md. The standard applies beginning with Phase 1, Step 2 and to future modifications of Step 1; the original Step 1 implementation is grandfathered. The example uses UDP request/response exchanges in accordance with the approved terminology. No implementation source changed, no new tests executed, no Step 2 work or milestone tag introduced. Documentation checked for completeness against the supplied text and for whitespace errors.

## 2026-11-11 — Synchronize user-added review artifacts

Added user-supplied project-doc/Notings.xlsx and project-doc/plantUML/FlowChart/ESP32-UDP Client.png. Added .gitignore to exclude Office lock files and updated FILE_REGISTRY.md. Workbook ZIP integrity checked; supplied Wi-Fi credential values were not found in workbook XML. Flowchart inspected as a review artifact. No firmware generated, no implementation tests performed, and no G2 approval or milestone acceptance implied.


## 2026-11-11 — Phase 1, Step 2 G3 implementation

Implemented the approved nested ESP-IDF project: project/component CMakeLists.txt, main/main.c and sdkconfig.defaults. Extended .gitignore; created the private credential header locally only. Updated all six phase records, ARCHITECTURE_DECISIONS.md and PHASE_ROADMAP.md. Exactly two custom functions: app_main and wifi_event_handler; built-in ESP-IDF/lwIP/FreeRTOS dependencies only. Single app_main connection after STARTED; one shared wrap-safe 30-second deadline; maximum 20 attempted exchanges; exact PING/PONG; stop on first failure; callback-safe conditional event-group deletion and guarded cleanup.

Build/size inspection completed successfully with verified ESP-IDF 5.5.4 and ccache disabled. Application binary 757920 bytes, bootloader 22272 bytes; application fits its 1 MiB factory partition with 28% free. No main.c compiler warnings emitted. Initial cache stall interrupted; process-scoped Git ownership settings fixed version discovery. Old unused-tool notice remains informational. Full evidence and hashes in EXPERIMENT_RESULTS.md. No firmware-design deviations, external dependencies or additional custom functions/tasks introduced.

No G4 tests, flashing, monitoring or network experiments executed. Existing NodeJS server, README and SDK unchanged. Independent G3 review and explicit G4 approval required before testing. No merge, tag or Step 3.
