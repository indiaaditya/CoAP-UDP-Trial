# Phase 2 file registry — G3-A actual files

All listed files are new under phase2-coap-baseline. Source/build inventory is confined to this worktree. Original phase1-udp-baseline files are retained unchanged. No uncommitted workbook/PNG copied. Generated credential-bearing artifacts are ignored, not release files.

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
| src/package.json | Exact coap production dependency; G3-C dev dependency not yet added |
| doc/FILE_REGISTRY.md | This inventory |
| doc/EXPERIMENT_RESULTS.md | Actual G3-A build, environment and check evidence |

Existing modified files: .gitignore (Phase 2 local/generated exclusions); project-doc/ARCHITECTURE_DECISIONS.md, PHASE_ROADMAP.md, EXPERIMENT_COMPARISON.md (approval/stage/evidence records). README, user Office notes/diagram and Phase 1 directory unchanged.

Local ignored: src/node_modules, src/esp32-coap-client/.private/wifi_credentials.h, sdkconfig, build/, managed_components/. Credentials are present in local firmware outputs; never publish binaries/ELF or private header.

Deferred and absent: main/Kconfig.projbuild, sdkconfig.scoped.defaults and full scoped/measurement implementation (G3-B); test/coap_fixture.js and direct dev coap-packet dependency (G3-C). No extra C/header/custom adapter.
