# Phase roadmap

Objectives follow the existing README and approved Step 1 instructions. Status as of 2026-11-10.

| Phase | Objective | Status |
| --- | --- | --- |
| 1 — UDP baseline | Minimal Node.js UDP server; embedded C client; device/cloud bidirectional validation | Step 1 source reviewed and acceptable; T07 passed by user-reported manual verification. Phase incomplete; Step 2 G2 approved and G3 source accepted; G4 attempted: five checks passed, boot test failed because provisioned flash encryption conflicts with the plaintext baseline; eight runtime tests not executed. Further deployment requires user direction/design approval. |
| 2 — CoAP feasibility | Minimal Node.js and embedded CoAP; measure flash, RAM, heap, stack and runtime overhead | Not started |
| 3 — Node.js emulator | Reproduce embedded behavior and verify against the same cloud utility | Not started |
| 4 — Secure communication | Device authentication, key establishment and authenticated encryption | Not started |

No subsequent implementation is authorized by Step 1. Proposed annotated milestone tag phase1-udp-baseline-v1.0 awaits complete Phase 1 review and acceptance.
