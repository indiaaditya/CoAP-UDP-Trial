# CoAP-UDP-Trial

## Objective

This repository contains a proof-of-concept implementation for evaluating a lightweight device-to-cloud communication path based on **UDP and CoAP**.

The objective is to determine whether CoAP can provide a practical, standards-based request/response mechanism for constrained embedded devices while keeping communication overhead, memory usage, and implementation complexity low.

The work will evaluate:

- Basic UDP communication between an embedded device and a Node.js server.
- CoAP request/response communication over UDP.
- Suitability of a minimal CoAP implementation for constrained FreeRTOS-based controllers.
- Flash, RAM, heap, and task-stack overhead introduced by the CoAP implementation.
- Functional equivalence between the embedded implementation and a Node.js device emulator.
- Integration of an authenticated and encrypted communication layer after the basic communication path has been validated.

## Development Approach

The work is divided into four incremental phases.

### Phase 1 — UDP Baseline

Establish the basic communication path without CoAP or security.

- Create a minimal Node.js UDP server utility.
- Create an embedded C UDP client.
- Verify bidirectional data exchange between the embedded device and the Node.js server.

**Exit condition:** Reliable UDP request/response communication is demonstrated.

### Phase 2 — CoAP Feasibility

Introduce CoAP while keeping the implementation intentionally minimal.

- Add CoAP support to the Node.js server.
- Add a minimal CoAP client implementation to the embedded C code.
- Verify CoAP request/response communication.
- Measure:
  - Flash usage.
  - Static RAM usage.
  - Heap usage.
  - Task-stack requirement.
  - Runtime overhead.

**Exit condition:** CoAP functionality is demonstrated and its embedded resource cost is documented.

### Phase 3 — Node.js Device Emulator

Create a software representation of the embedded device.

- Implement `coap_device.js`.
- Reproduce the relevant behavior of the embedded C implementation.
- Verify `coap_device.js` against the same Node.js cloud utility used during the embedded trials.

**Exit condition:** The Node.js device emulator reproduces the required embedded communication behavior for the defined trial scope.

### Phase 4 — Secure End-to-End Communication

Add the security framework after the communication path is stable.

- Integrate device authentication and secure key establishment.
- Add authenticated encryption for application data.
- Verify secure end-to-end request/response communication between `coap_device.js` and `cloud.js`.
- Retain the resulting implementation as a reference for subsequent embedded integration.

**Exit condition:** A complete secure communication path is demonstrated and documented.

## Design Principles

The implementation will remain deliberately small and traceable.

- Standard protocols are preferred where practical.
- Only the protocol features required for the trial will be enabled.
- Protocol feasibility and security integration will be evaluated separately.
- Embedded resource measurements will be recorded before architectural conclusions are made.
- Each source file, major function, configuration item, and protocol element should have a clearly documented purpose.
- New functionality should be introduced incrementally so that its effect on code size and runtime resources can be measured.

## Expected Outcome

At the end of the trial, the repository should provide:

1. A working UDP baseline.
2. A working minimal CoAP implementation.
3. Measured embedded resource requirements.
4. A Node.js device emulator matching the required embedded behavior.
5. A secure end-to-end reference implementation.
6. Sufficient technical evidence to decide whether the selected CoAP-based approach is suitable for further integration.

## Status

Work in progress.
