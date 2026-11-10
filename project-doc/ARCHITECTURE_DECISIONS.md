# Architecture decision history

## ADR-001 — Minimal UDP baseline (2026-11-10, approved)

Use a single Node.js file with built-in dgram, udp4, default 0.0.0.0:56830, exact ASCII PING/PONG and replies to the observed sender. Configure the port with one source constant. This isolates basic UDP behavior without dependencies, persistent application state or future-phase functionality.

## ADR-002 — Initial organization (2026-11-10, superseded)

The original proposal used root-level source and registries. The first addendum replaced that layout with root src/, doc/, test/ and rls/ and branch Phase1-UF. These layouts were superseded before implementation by ADR-003; neither was implemented.

## ADR-003 — Phase directories and milestone history (2026-11-10, approved)

Supersedes ADR-002. The user selected phase1-udp-baseline as both development branch and phase directory, replacing Phase1-UF. Phase source and documentation reside under that directory; project-doc/ contains cross-phase records. README.md remains unchanged. Reserved test/ and rls/ contain only .gitkeep. This preserves identifiable experiments for future comparison.

Later phases use phase-specific branches from approved baselines and retain earlier phase directories. No future-phase directories are created now. The proposed annotated tag phase1-udp-baseline-v1.0 must await review and acceptance of the complete Phase 1 milestone. No Step 1 tag is created. Corrections must preserve existing acceptance tags and historical test evidence. No merge is authorized.
