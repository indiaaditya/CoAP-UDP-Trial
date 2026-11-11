# Architecture decision history

## ADR-001 — Minimal UDP baseline (2026-11-10, approved)

Use a single Node.js file with built-in dgram, udp4, default 0.0.0.0:56830, exact ASCII PING/PONG and replies to the observed sender. Configure the port with one source constant. This isolates basic UDP behavior without dependencies, persistent application state or future-phase functionality.

## ADR-002 — Initial organization (2026-11-10, superseded)

The original proposal used root-level source and registries. The first addendum replaced that layout with root src/, doc/, test/ and rls/ and branch Phase1-UF. These layouts were superseded before implementation by ADR-003; neither was implemented.

## ADR-003 — Phase directories and milestone history (2026-11-10, approved)

Supersedes ADR-002. The user selected phase1-udp-baseline as both development branch and phase directory, replacing Phase1-UF. Phase source and documentation reside under that directory; project-doc/ contains cross-phase records. README.md remains unchanged. Reserved test/ and rls/ contain only .gitkeep. This preserves identifiable experiments for future comparison.

Later phases use phase-specific branches from approved baselines and retain earlier phase directories. No future-phase directories are created now. The proposed annotated tag phase1-udp-baseline-v1.0 must await review and acceptance of the complete Phase 1 milestone. No Step 1 tag is created. Corrections must preserve existing acceptance tags and historical test evidence. No merge is authorized.


## ADR-004 — Step 2 approved G2 firmware design (2026-11-11)

G1 and complete G2 were explicitly approved before G3. Use the existing restored ESP-IDF v5.5.4 at commit 735507283d5b2f9fb363a1901172dbd9e847945d; no isolated installation is required after tracked SDK changes were reverted. Unrelated untracked SDK projects remain untouched.

Implement only app_main and wifi_event_handler in the approved nested ESP-IDF project. app_main is the sole connection owner, waits for STARTED, and uses one wrap-safe 30-second startup/IPv4 budget. FAILED is latched. Use bundled lwIP connected IPv4 UDP with at most 20 attempted exchanges, exact PING/PONG, two-second receive timeout, no retry/reconnect/custom task. Retain the existing server unchanged.

Stop Wi-Fi, unregister application callbacks and conditionally delete their event group before continuing guarded teardown. On failed unregistration, retain callback-reachable resources until reset. Private credentials are local ignored header macros and firmware outputs remain ignored. Physical 8 MB PSRAM is disabled; flash configured 16 MB QIO at 40 MHz; 40 MHz crystal; 4096-byte main stack; UART at 115200. G3 authorizes build only; G4 flashing/runtime tests need a separate explicit approval. No phase advancement, merge or tag.


## ADR-005 — Explicitly approved target recovery (2026-11-11)

After the G4 boot failure exposed a provisioned flash-encryption state, the user explicitly approved advancing SPI_BOOT_CRYPT_CNT from 0b001 to 0b011, a full-chip erase, and reflashing the already approved plaintext firmware. This is a deployment/hardware-state exception to the prior no-eFuse-change/no-whole-chip-erase baseline, not a firmware design change.

Preflight confirmed the counter was 0b001 and writable, Secure Boot was disabled, and the approved generated firmware had flash encryption disabled. Only the requested counter field was burned to 0b011; readback reported flash encryption disabled. The full-chip erase completed and all three firmware image writes were hash-verified. Other existing security state, including provisioned key purpose and permanently disabled JTAG, was not altered. The board subsequently booted ESP-IDF v5.5.4 and app_main successfully. Historical failed-boot evidence is retained.
