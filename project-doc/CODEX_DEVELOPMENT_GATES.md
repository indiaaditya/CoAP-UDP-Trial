# Project-Wide Codex Pre-Implementation Design Standard

This standard applies to **all future Codex-generated implementation work** in this project unless a specific task is explicitly exempted by the user.

It applies across all phases and technologies, including:

- NodeJS utilities;
- embedded C firmware;
- UDP communication;
- CoAP integration;
- security framework integration;
- test utilities;
- future protocol refinements.

The objective is to maintain complete visibility and control over the codebase as it grows.

---

# G1 — Scope and Requirements Gate

Before proposing implementation details, Codex must present:

1. Implementation objective.
2. Current phase and step.
3. Relevant hardware assumptions.
4. Relevant software/platform assumptions.
5. Files proposed to be created or modified.
6. Major responsibility of each file/module.
7. Explicit in-scope functionality.
8. Explicit out-of-scope functionality.
9. Dependencies on previously implemented functionality.

Codex must obtain user approval before proceeding to G2.

---

# G2 — Detailed Design Gate

Before writing or modifying implementation source code, Codex must present the complete proposed design.

## 1. Function Registry Proposal

For every proposed custom function provide:

- function name;
- return type;
- parameters and exact types;
- purpose;
- caller;
- major functions/APIs called;
- expected success result;
- expected failure result.

Clearly distinguish:

- **Custom project functions**
- **Framework/library functions**
- **ESP-IDF / SDK functions**
- **NodeJS built-in or package functions**

No implementation function should appear later unless it was approved here or subsequently approved as a design change.

---

## 2. Variable and Constant Proposal

For every architecturally significant variable or constant provide:

- exact name;
- exact type;
- scope;
- storage/lifetime;
- purpose;
- initialization/default value where applicable;
- owner/function/module.

Include:

- global variables;
- static variables;
- configuration constants;
- protocol fields;
- state variables;
- timers/counters;
- buffers whose size is architecturally significant.

Routine local temporary variables need not be exhaustively listed unless relevant to the design.

---

## 3. Libraries and Dependencies

List every proposed:

- header;
- library;
- SDK component;
- ESP-IDF component;
- NodeJS module/package;
- external dependency.

For each dependency explain:

- why it is required;
- what functionality it provides;
- whether it is built-in, SDK-provided or external;
- whether a simpler existing project dependency can satisfy the requirement;
- relevant resource or maintenance implications.

No unapproved external dependency may be added during implementation.

---

## 4. File and Module Organization

Show:

- proposed files;
- purpose of each file;
- ownership of functions;
- relationship between modules;
- which existing files will be modified.

Avoid unnecessary abstraction or file proliferation.

---

## 5. Execution Flow

Provide a **PlantUML flowchart** showing:

- startup/entry point;
- major function calls;
- decisions;
- success paths;
- failure paths;
- retries where applicable;
- return/exit behavior.

Function names shown in the flowchart must correspond to the proposed function list.

---

## 6. Error-Handling Strategy

Define expected handling for relevant failures, such as:

- initialization failure;
- network failure;
- socket failure;
- timeout;
- malformed packet;
- unexpected response;
- cryptographic failure;
- resource allocation failure.

The strategy should be proportional to the current phase.

Do not introduce production-grade recovery architecture unless the phase requires it.

---

## 7. Resource Implications

Where relevant, identify expected impact on:

- Flash/code size;
- static RAM;
- heap;
- task stack;
- buffers;
- sockets;
- CPU processing;
- network traffic.

For feasibility trials, distinguish:

**expected resource implication**

from

**measured resource result**.

---

## 8. Test Plan and Acceptance Criteria

Before implementation, define:

- tests to be performed;
- expected input;
- expected output;
- expected logs;
- pass/fail criteria;
- measurement requirements.

Testing must correspond directly to the approved implementation objective.

---

## G2 Approval Rule

Codex must discuss the proposed design with the user and incorporate requested corrections.

**No implementation source code may be written or modified until the user explicitly approves G2.**

Approval of G2 means approval of the design, not automatic approval of future deviations.

---

# G3 — Implementation Gate

After G2 approval, Codex may implement the approved design.

Implementation must conform to:

- approved files;
- approved functions;
- approved variables/constants;
- approved dependencies;
- approved execution flow;
- approved error-handling strategy.

Codex must not silently introduce:

- new functions;
- new files;
- new dependencies;
- additional abstraction layers;
- new state variables;
- retry mechanisms;
- helper frameworks;
- protocol changes.

If implementation reveals that a design change is necessary, Codex must:

1. stop the affected implementation;
2. describe the required deviation;
3. explain why it is necessary;
4. update the relevant G2 design information;
5. obtain user approval;
6. then continue implementation.

---

# Documentation Update During G3

After implementation, Codex must update the relevant project records, including where applicable:

- `FILE_REGISTRY.md`
- `FUNCTION_REGISTRY.md`
- `VARIABLE_REGISTRY.md`
- `PROTOCOL_REGISTRY.md`
- `CHANGE_LOG.md`

The registries must describe the **actual implementation**, not only the original proposal.

Any approved deviation must be visible.

---

# G3 Completion Review

After implementation, Codex must provide:

1. Files created or modified.
2. Functions actually implemented.
3. Variables/constants actually introduced.
4. Dependencies actually used.
5. Deviations from the approved G2 design.
6. Build result.
7. Known warnings or unresolved issues.

The completed implementation should then be available for independent ChatGPT review before acceptance.

---

# G4 — Testing and Acceptance Gate

Execute only the tests approved under G2, plus any additional tests explicitly approved after implementation.

Record factual results.

For every test provide:

- test identifier;
- purpose;
- input/setup;
- expected result;
- actual result;
- PASS/FAIL;
- relevant log/output;
- observations.

If defects are identified:

```text
Test failure
    ↓
Root-cause investigation
    ↓
Proposed correction
    ↓
Design impact review
    ↓
Implementation correction
    ↓
Retest
```

A correction that changes the approved design must return through the appropriate G2/G3 approval process.

---

# Evidence vs Architectural Conclusion

Codex must report measurements and observations factually.

Examples:

**Allowed**

> CoAP integration increased firmware size by 42 kB.

> Peak free heap reduced by 11 kB during the test.

> 100 consecutive UDP request/response exchanges completed with 99 successful responses.

**Not allowed as an implementation conclusion**

> CoAP is too large for the product.

> Custom UDP should therefore be selected.

> This architecture is better than MQTT.

Architectural interpretation remains with the user and Master Architecture Chat.

---

# No Automatic Phase Advancement

Completion of G4 does not authorize Codex to begin another:

- step;
- feature;
- phase;
- protocol;
- optimization.

The next activity begins only after explicit user direction.

---

# Repository Ownership Rule

All implementation code and repository changes are performed by **Codex only**.

ChatGPT acts as:

- architect;
- reviewer;
- Codex instruction generator;
- design evaluator;
- test-result evaluator.

ChatGPT must not push or modify repository files.

---

# Applicability

This standard applies beginning with:

**Phase 1, Step 2**

Phase 1, Step 1 — the original minimal NodeJS UDP server — is grandfathered and does not require retrospective redesign.

However, any future modification to that implementation must follow this standard.