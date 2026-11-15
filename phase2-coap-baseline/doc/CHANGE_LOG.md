# Phase 2 change log

## 2026-11-15 — G3-A implementation candidate

User approved complete G2 plus final-ACK/callback-return/controlled-sequencing clarifications and authorized G3-A only. Created phase2-coap-baseline worktree branch from ac1fa50d6731fd3f607789862fa28fadd8bf84e4. Original Phase 1 working tree, branch and uncommitted workbook/PNG preserved; no stashing/staging/commit/reset/clean/tag/merge/push.

Implemented separate Node CoAP server with four approved callbacks and official component-based ESP32 client with five approved functions. Preserved Phase 1 Wi-Fi initialization/shared deadline/failure latch/guarded cleanup; no new task, raw transport layer or adapter. Added exact manifests/locks, approved feature selections and essential B1/B2/B3/B5/B6/whole-run logs. Reused committed effective non-secret Phase 1 configuration and privately copied only the existing credential header into the new ignored private directory. No uncommitted project note/image was copied.

Full measurement Kconfig/scoped build, runtime campaign and host fixture/test-only dependency are deferred to G3-B/C. Updated actual registries, design record and cross-phase status. Build/dependency/check outcomes are in EXPERIMENT_RESULTS.md. No flashing/device access or functional network tests. No architectural/security expansion. Pending independent source review; no advance beyond G3-A.


## G3-A acceptance and G3-B resource instrumentation

G3-A was separately committed/pushed as aad119d4a980b37e510940e37a85b835a1630717 and independently accepted at source/build-review level. G3-B only is now authorized. Added the approved primary/scoped Kconfig arrangement, stored checkpoints, task counts, monitor bookkeeping observations and scoped B4 lifecycle inside app_main. No new custom function/task/dependency or protocol/security behavior. Communication handlers, campaign and cleanup preserved. Separate clean build-primary/build-scoped output/config directories; retained G3-A and Phase1 artifacts unchanged. Linker analysis explains the148688-byte G3-A whole-build increase and distinguishes archive retention, shared/security backend and linker accounting. No optimization or UDP rebuild. Runtime checkpoints remain unmeasured pending G4. Corrected stale uncommitted/review status documentation. G3-B was subsequently committed/pushed and accepted as c146cb2353feb5cc467f85968fb9f6546fb1f607. G3-C implementation is now authorized; G4/merge/tag remain unauthorized.


## G3-B acceptance and G3-C host fixture candidate

G3-B committed/pushed and accepted at source/build-review level as c146cb2353feb5cc467f85968fb9f6546fb1f607. Implemented approved host-only fixture and exact coap-packet1.1.1 dev dependency; unchanged production server, firmware and dependency versions. Updated registries/status/evidence and corrected23-to24-byte request arithmetic (no wire/protocol change). Syntax checks passed; VM/mock sockets/timers yielded48/49 initially, the sole erroneous expected-size assertion was corrected and retested, resolving49/49; unchanged-server mock callback checks14/14. No actual socket, network or hardware test. Detailed case ledger in EXPERIMENT_RESULTS.md. No G3-C commit/push authorization; stopped for independent review. G4/merge/tag remain unauthorized.


## Approved180-second watchdog amendment and G4 procedure finalization

G3-C accepted at7dbe6f2298fbdcbeee34669c097ca55648b22f71. Changed exactly FIXTURE_TIMEOUT_MS120000 ->180000; no other fixture source/function/mode/dependency/socket/timing change, no server/ESP modification. Updated current validation expectation;27/27 applicable VM mocked checks pass with real public codec and in-memory sockets/timers. Historical120s G3-C validation remains explicitly historical, not relabeled as a180s result. Finalized existing P2-01..29 procedure with60s operator readiness, capture proof prerequisites, immutable authoritative artifact pairs and separate five-field reporting. No capture tool installed/used, no actual socket or hardware, no firmware rebuild/flash, no commit/push/merge/tag. Stopped for independent amendment review.

## G4-A source-only corrective amendment - uncommitted review candidate

Based on accepted f7d0f1682d835611f61b658ff2e60b4607ac704d. Historical G4-A: P2-02..05 PASS retained; P2-01 BLOCKED capture readiness; P2-06 FAIL shared bind; P2-07/P2-08 BLOCKED. Read-only investigation identified default reuseAddr=true as the P2-06 cause and a high-confidence outgoing-response send gate explanation for P2-07. User separately approved adding reuseAddr=false and setting sendAcksForNonConfirmablePackets=true in cloud.js only. Four callback bodies unchanged. Syntax/static checks and 25/25 VM mocked checks pass, including original 14 application request cases. Source options permit intended behavior but do not establish real Windows/UDP results. Documentation updated; no historical failure relabeled. No firmware rebuild, fixture/dependency/lock change, live socket/network/hardware access, software installation, system configuration, commit or push. Stop for source review before separately authorized retests.
