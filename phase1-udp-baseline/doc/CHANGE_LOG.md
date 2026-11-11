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


## 2026-11-11 — Step 2 G4 hardware attempt and boot failure

G3 source accepted; G4 authorized. Executed S2-T01-T05 and S2-T14: five PASS and boot-stage FAIL for T05. Verified flash writes after manual download entry, but ROM rejected the boot header. Read-only security inspection reported flash encryption enabled, SPI_BOOT_CRYPT_CNT=0x1 and an XTS_AES_128 key purpose, conflicting with the approved plaintext deployment baseline. T06-T13 were not executed. Recorded factual logs, G4 binary hash, limitations and static-review scope in EXPERIMENT_RESULTS.md. Updated PHASE_ROADMAP.md. No source/SDK/security-state changes, whole-chip erase, AP changes, merge, tag or phase advancement. Stopped the test-owned UDP server and left the board in download mode. Correction requires user direction/design approval before further writes.


## 2026-11-11 — Authorized board recovery and completed available G4 tests

User explicitly approved counter burn SPI_BOOT_CRYPT_CNT 0b001 -> 0b011, full-chip erase and reflash. Verified flash encryption disabled, erased and hash-verified all firmware writes; plaintext boot succeeded. User corrected private SSID capitalization; changed only the ignored credential header, rebuilt/reflashed, preserving tracked firmware source. Existing security key purpose/JTAG state unchanged. Added ADR-005 for this approved deployment exception.

Final G4 results: 12 PASS, S2-T11/S2-T12 NOT EXECUTED by user direction. Two positive runs delivered 20/20 exact PONG each (40/40 combined); server-absent, three malformed-response and wrong-source-port cases each stopped at 1/0 as expected. Preserved original ROM failure, initial unavailable-network run, and an association-failed server-absence setup run (0 attempts) separately. Recorded runtime heap/stack observations, final binary hash and actual logs in EXPERIMENT_RESULTS.md. Updated PHASE_ROADMAP.md and EXPERIMENT_COMPARISON.md. No source/SDK/server changes, extra functions/tasks/dependencies, merge, milestone tag or Step 3 work. Test-owned server/responders stopped. Results await independent review; no automatic phase acceptance.


## 2026-11-11 — Phase 1 Step 3 documentation closure candidate

Audit input branch phase1-udp-baseline at b13620fe2b75084bd31fe5869da57c00beef8049, initially clean and synchronized with origin. Reviewed all 21 tracked files and accepted NodeJS/C sources against file/function/variable/protocol registries. Corrected the stale historical diagram approval label and clarified Step 1 function-registry scope. Appended audit outcomes to all four registries; no implementation changes.

Added the concise completion report ahead of existing EXPERIMENT_RESULTS.md history, preserving historical failure/success observations. Archived complete effective non-secret sdkconfig, pinned SDK submodules, source/artifact hashes, existing resource measurements, unexecuted tests, coverage limitations and acceptance recommendation in the same document. Consolidated already approved G2 startup/cleanup revisions and PlantUML in ARCHITECTURE_DECISIONS.md without inventing a new decision; retained ADR-002 supersession and ADR-005 deployment exception. Updated PHASE_ROADMAP.md and EXPERIMENT_COMPARISON.md.

Validation was documentation/source/artifact inspection only: configuration and retained final binary hashes matched existing evidence; SDK tracked diff empty; submodule revisions pinned; no build or hardware test rerun. Source trees, README, SDK, Office notes and historical PNG unchanged. No device access, eFuse change, flash/erase, new files, tag, merge or Phase 2 work. Recommend independent review/user acceptance with the two AP tests explicitly unexecuted; do not treat recommendation as acceptance.


## 2026-11-11 — Dedicated ESP32 resource baseline documentation

Created doc/RESOURCE_BASELINE.md and registered it in FILE_REGISTRY.md. Consolidated existing ESP-IDF build-section data, final/earlier image identities, both successful hardware-run heap/stack readings, supplemental negative/setup-run readings, configured network settings, existing timestamps and traceable arithmetic. Added standardized Phase 1 comparison parameters/units, explicit Not Measured gaps and future CoAP subtraction conditions without architectural conclusions.

Provenance qualifications: detailed section sizes belong to the earlier final-source G3 build, not a separately measured final credential-corrected image; earlier local size output has a different Flash Code value and is kept distinct. Image hashes/metadata differ across original and retested images. No historical experiment result was edited. Validation is documentation/traceability/arithmetic and changed-file-scope inspection only, not new resource measurement. Only RESOURCE_BASELINE.md, FILE_REGISTRY.md and this log changed. No source/SDK/configuration/binary changes, rebuild, device access, flash/erase, hardware test, instrumentation, tag or Phase 2 work.


## 2026-11-11 — B0–B6 checkpoint schema and existing-map UDP attribution

Updated RESOURCE_BASELINE.md and FILE_REGISTRY.md to add requested checkpoint definitions. B1/B2/B5/B6 use recorded values; B0 clean-build provenance, B3 pre-first-send heap and B4 campaign-only minimum are unavailable and are not replaced by nearby readings. Existing evidence IDs B1/B2 renamed E-BUILD/E-CONFIG to avoid checkpoint collisions. All prior measurements retain their original values and scopes.

Analyzed the retained ESP32 linker map with the existing esp_idf_size parser: udp.c.obj attribution is 3708 flash bytes and 6 static RAM bytes, excluding main.c, shared infrastructure and dynamic allocations. Shared socket/API objects listed separately, never summed as UDP increment. SDK DHCP source and map inclusion show pre-existing network-stack UDP dependencies. No Wi-Fi-only reference or Beken artifacts are available, so incremental library cost remains unmeasured. No source/config/SDK/binary changes, builds/relinks, new hardware measurements, instrumentation, device actions, tags or phase advancement. Only resource baseline, file registry and change log changed.
