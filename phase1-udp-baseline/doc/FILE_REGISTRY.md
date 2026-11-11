# File registry

Paths are repository-relative. All listed files are retained; introduced in Phase 1, Step 1 unless stated otherwise. Temporary command-line test clients were not saved.

| File | Purpose |
| --- | --- |
| README.md | Existing objectives and roadmap; predates Step 1; unchanged |
| phase1-udp-baseline/src/cloud.js | Minimal IPv4 UDP server |
| phase1-udp-baseline/doc/FILE_REGISTRY.md | File purpose, introduction and retention |
| phase1-udp-baseline/doc/FUNCTION_REGISTRY.md | Significant callbacks |
| phase1-udp-baseline/doc/VARIABLE_REGISTRY.md | Significant configuration and variables |
| phase1-udp-baseline/doc/PROTOCOL_REGISTRY.md | Exact trial packet definitions |
| phase1-udp-baseline/doc/CHANGE_LOG.md | Change and test history |
| phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md | Run procedure and factual observations |
| phase1-udp-baseline/test/.gitkeep | Preserve reserved test directory; no functional content |
| phase1-udp-baseline/rls/.gitkeep | Preserve reserved release directory; no functional content |
| project-doc/ARCHITECTURE_DECISIONS.md | Cross-phase decision history |
| project-doc/PHASE_ROADMAP.md | Approved objectives and status |
| project-doc/EXPERIMENT_COMPARISON.md | Evidence-based comparisons |
| project-doc/CODEX_DEVELOPMENT_GATES.md | Permanent project-wide G1–G4 design, implementation and testing standard; introduced after Step 1 review; retained |
| project-doc/Notings.xlsx | User-maintained project notes workbook; introduced during Step 2 G2 review; retained |
| project-doc/plantUML/FlowChart/ESP32-UDP Client.png | User-supplied rendering of the proposed Step 2 flowchart; retained as a historical pre-final-revision rendering; final G2 was approved before G3 |
| .gitignore | Excludes Office temporary lock files; introduced during Step 2 G2 review; retained |


## Phase 1, Step 2 — G3 implementation

All paths below are repository-relative. Retained tracked files were introduced in Step 2; existing files are updated in place.

| File | Purpose / retention |
| --- | --- |
| phase1-udp-baseline/src/esp32-udp-client/CMakeLists.txt | ESP-IDF project build entry; tracked |
| phase1-udp-baseline/src/esp32-udp-client/main/CMakeLists.txt | Main component, built-in dependencies, mandatory private credential header; tracked |
| phase1-udp-baseline/src/esp32-udp-client/main/main.c | Two-function Wi-Fi/UDP firmware; tracked |
| phase1-udp-baseline/src/esp32-udp-client/sdkconfig.defaults | Non-secret ESP32-S3, flash, console and main-stack defaults; tracked |
| phase1-udp-baseline/src/esp32-udp-client/.private/wifi_credentials.h | Local credential macros; retained locally, explicitly ignored and never tracked |
| phase1-udp-baseline/src/esp32-udp-client/sdkconfig | Generated effective non-secret build configuration; local, ignored |
| phase1-udp-baseline/src/esp32-udp-client/build/ | Generated build outputs and logs, including credential-bearing ELF/binaries; local, ignored; not release artifacts |
| .gitignore | Extended to exclude private credentials, generated configuration and build output |

Updated FILE_REGISTRY.md, FUNCTION_REGISTRY.md, VARIABLE_REGISTRY.md, PROTOCOL_REGISTRY.md, CHANGE_LOG.md and EXPERIMENT_RESULTS.md, plus project-doc/ARCHITECTURE_DECISIONS.md and project-doc/PHASE_ROADMAP.md. Existing cloud.js, README.md and SDK source are unchanged. Reserved test/ and rls/ still contain only .gitkeep. The user-added diagram remains a historical proposal rendering; final startup/cleanup revisions were approved in the design review before G3.


## Step 3 milestone-documentation audit — 2026-11-11

All 21 tracked files at input commit b13620fe2b75084bd31fe5869da57c00beef8049 are covered by the registry above. README and Office/diagram artifacts are retained unchanged. Registries cover the current NodeJS server and four ESP-IDF project files; .private/, generated configuration and credential-bearing outputs remain local and ignored. No new file is introduced for closure: EXPERIMENT_RESULTS.md contains the completion report, source/artifact identity table and complete effective non-secret sdkconfig snapshot; ARCHITECTURE_DECISIONS.md contains the consolidated approved G2 record and final flowchart. These are documentation records, not additional implementation or release artifacts. No earlier failure observation has been removed.


## Dedicated resource baseline — 2026-11-11

| File | Purpose / introduction / retention |
| --- | --- |
| phase1-udp-baseline/doc/RESOURCE_BASELINE.md | Dedicated raw-UDP ESP32 resource record for future like-for-like comparisons; introduced by the user-authorized documentation-only resource-baseline task after Step 3; retained, tracked |

Consolidates existing build/runtime evidence, explicit derived values and unmeasured gaps, with source/run/image provenance. Does not modify historical observations or authorize new instrumentation, builds, device actions or tests.


Resource-baseline revision (2026-11-11): RESOURCE_BASELINE.md now defines B0–B6 checkpoints and reserves their names separately from evidence IDs E-BUILD/E-CONFIG. It also records read-only retained-linker-map library attribution, explicitly distinguished from an unavailable incremental Wi-Fi-to-UDP or Beken footprint. No new project file, firmware instrument or test artifact created.

## Manual trial procedure — 2026-11-11

| File | Purpose / introduction / retention |
| --- | --- |
| phase1-udp-baseline/doc/MANUAL_TEST_PROCEDURE.md | User-supplied revision 1.0 manual ESP32/NodeJS UDP trial procedure and observation-record template; introduced by explicit documentation request; retained, tracked |

Procedure text is preserved as supplied. It describes MT-01 through MT-05; adding it does not execute or record new trials. Canonical recorded results remain in EXPERIMENT_RESULTS.md and checkpoint/image provenance remains in RESOURCE_BASELINE.md.
