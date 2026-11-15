# Phase 2 file registry — G3-A actual files

Listed files were introduced under phase2-coap-baseline in accepted G3-A unless identified below as G3-B additions. Source/build inventory is confined to this worktree. Original phase1-udp-baseline files are retained unchanged. No uncommitted workbook/PNG copied. Generated credential-bearing artifacts are ignored, not release files.

| File (phase-relative) | Responsibility |
| --- | --- |
| doc/CHANGE_LOG.md | Phase 2 design/actual registry/evidence record |
| doc/FUNCTION_REGISTRY.md | Phase 2 design/actual registry/evidence record |
| doc/G2_DESIGN.md | Phase 2 design/actual registry/evidence record |
| doc/MANUAL_TEST_PROCEDURE.md | Phase 2 design/actual registry/evidence record |
| doc/PROTOCOL_REGISTRY.md | Phase 2 design/actual registry/evidence record |
| doc/RESOURCE_BASELINE.md | Phase 2 design/actual registry/evidence record |
| doc/VARIABLE_REGISTRY.md | Phase 2 design/actual registry/evidence record |
| src/cloud.js | Four Node server callbacks; minimal CoAP validation/reply |
| src/esp32-coap-client/CMakeLists.txt | ESP-IDF project entry |
| src/esp32-coap-client/dependencies.lock | Resolved official component hash/version |
| src/esp32-coap-client/main/CMakeLists.txt | Main source, private header guard and explicit requirements |
| src/esp32-coap-client/main/idf_component.yml | Exact official component and SDK requirement |
| src/esp32-coap-client/main/main.c | Five custom firmware functions |
| src/esp32-coap-client/sdkconfig.defaults | Complete committed non-secret baseline settings plus CoAP options |
| src/package-lock.json | Exact resolved npm dependency tree/integrities |
| src/package.json | Exact coap production dependency and G3-C coap-packet1.1.1 dev dependency |
| src/esp32-coap-client/main/Kconfig.projbuild | G3-B: scoped-monitor bool, default n |
| src/esp32-coap-client/sdkconfig.scoped.defaults | G3-B: supplemental y override |
| test/coap_fixture.js | G3-C: approved host-only fault utility, eleven functions |
| doc/FILE_REGISTRY.md | This inventory |
| doc/EXPERIMENT_RESULTS.md | Actual G3-A build, environment and check evidence |

Existing modified files: .gitignore (Phase 2 local/generated exclusions); project-doc/ARCHITECTURE_DECISIONS.md, PHASE_ROADMAP.md, EXPERIMENT_COMPARISON.md (approval/stage/evidence records). README, user Office notes/diagram and Phase 1 directory unchanged.

Local ignored: src/node_modules, src/esp32-coap-client/.private/wifi_credentials.h, sdkconfig, build/, managed_components/. Credentials are present in local firmware outputs; never publish binaries/ELF or private header.

G3-B now adds src/esp32-coap-client/main/Kconfig.projbuild and src/esp32-coap-client/sdkconfig.scoped.defaults, and modifies main/main.c and sdkconfig.defaults for approved instrumentation. G3-C now adds test/coap_fixture.js and direct dev coap-packet1.1.1; production server and firmware remain unchanged. No extra C/header/custom adapter.


G3-A accepted commit: aad119d4a980b37e510940e37a85b835a1630717. G3-B generated outputs are separate ignored build-primary/ and build-scoped/ directories, each with its own sdkconfig, binary, ELF, map, bootloader, logs and size reports. Retained build/ holds unchanged G3-A artifacts and read-only historical attribution reports. No generated or private file is intended for Git. Current source/build evidence is in RESOURCE_BASELINE.md and EXPERIMENT_RESULTS.md. All existing source/configuration lock dependencies unchanged.


## G3-C actual additions/updates

New test/coap_fixture.js: eleven approved host-only functions and sixteen modes, no production server modification. Updated src/package.json and src/package-lock.json: exact direct devDependency coap-packet1.1.1, already installed transitively by coap; all resolved package versions remain unchanged. No extra runtime library/framework.

Updated function/variable/protocol/file registries, approved-design/status/change/experiment/manual/resource records and applicable project-doc records. Local validation JSON reports are ignored under src/esp32-coap-client/build/g3c-validation.json and g3c-server-validation.json; no new persistent test harness source was added. No generated binaries, credential header, device access, G4 run, commit or push.


## Watchdog-only amendment review inventory

Source: test/coap_fixture.js, exactly one constant changed120000 ->180000. No firmware/server/manifest/lock change. Documentation: MANUAL_TEST_PROCEDURE.md finalized for P2-01..29 with capture prerequisites, authoritative immutable primary/scoped ledgers, first-request60s rule,180s host ceiling and separate outcome fields; relevant function/variable/protocol/design/change/experiment/file records updated. Local generated build/watchdog-amendment-validation.json remains ignored; no persistent harness source or new implementation function.
