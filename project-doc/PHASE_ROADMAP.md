# Phase roadmap

Objectives follow the existing README and approved Step 1 instructions. Status as of 2026-11-11.

| Phase | Objective | Status |
| --- | --- | --- |
| 1 — UDP baseline | Minimal Node.js UDP server; embedded C client; device/cloud bidirectional validation | Step 1 source reviewed and acceptable; T07 passed by user-reported manual verification. Phase incomplete; Step 2 G2 approved and G3 source accepted. User-approved target recovery completed; G4 available suite: 12 PASS, 2 AP tests not executed by user direction. Step 3 documentation consolidation completed; candidate milestone report submitted for independent review and user acceptance. Phase 1 is not yet accepted. |
| 2 — CoAP feasibility | Minimal Node.js and embedded CoAP; measure flash, RAM, heap, stack and runtime overhead | Not started |
| 3 — Node.js emulator | Reproduce embedded behavior and verify against the same cloud utility | Not started |
| 4 — Secure communication | Device authentication, key establishment and authenticated encryption | Not started |

Step 3 authorizes review/documentation only. No Phase 2 implementation is authorized. Proposed annotated milestone tag phase1-udp-baseline-v1.0 awaits complete Phase 1 review and acceptance.


The completion report and full non-secret configuration archive are in phase1-udp-baseline/doc/EXPERIMENT_RESULTS.md. The approved final G2 design is consolidated in ARCHITECTURE_DECISIONS.md. S2-T11 and S2-T12 remain unexecuted by user direction; no test rerun was authorized for Step 3. Final acceptance and any annotated phase1-udp-baseline-v1.0 tag require separate explicit user decisions. No tag or merge has been performed.
