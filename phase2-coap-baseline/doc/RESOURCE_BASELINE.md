# Phase 2 resource record — G3-A build only

No hardware measurements, primary/scoped campaign or G3-B size-attribution study has been performed. Build size/hash evidence is recorded in EXPERIMENT_RESULTS.md. Essential approved firmware logging exists only for B1, B2, B3, B5, B6 and the whole-run watermark.

| Checkpoint | Placement / availability |
| --- | --- |
| B0 | G3-A build uses previously nonexistent build directory; actual binary identity in experiment record |
| B1 | Before NVS/network/Wi-Fi initialization; free internal 8-bit heap |
| B2 | After IPv4, before coap_startup/context/session |
| B3 | After session/token setup, before first request construction |
| B4 | Not implemented/measured; deferred to authorized G3-B supplemental build |
| B5 | Result logging, immediately before CoAP cleanup |
| B6 | Main-task high-water mark at result; bytes under ESP-IDF |
| Whole-run minimum | heap_caps_get_minimum_free_size; includes startup/network history; not B4 |

All runtime values are Not Measured. No zero substitution. Pool MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT. Existing SDK concurrent activity is included. Init failures never reaching B2/B3 do not fabricate readings. B5/B6 logs can still occur on failure before cleanup.

Historical comparator: repository directory phase1-udp-baseline at ac1fa50d6731fd3f607789862fa28fadd8bf84e4 (see the actual repository phase1-udp-baseline/doc/RESOURCE_BASELINE.md). Historical binary 757920 bytes; B1 329724/329724, B2 278212/278200, B5 277576/277576, B6 1748/1748 bytes. Whole-run minimum 275460/274008. Historical B0 clean-build provenance, B3 and campaign-only B4 missing. Detailed sections derive from an earlier image than final successful hardware runs.

Future G3-B: complete primary/scoped instrumentation, separately identified build outputs, monitor bookkeeping overhead, component/file attribution and approved measurement records. Scoped monitor allocates memory and resets minima; do not compare its watermark as historical whole-run minimum. The pool minimum aggregates per-heap minima and is not necessarily simultaneous global free memory.

Compare whole-build deltas and shared infrastructure, CoAP library, integration and instrumentation separately. Free-heap delta sign is CoAP free minus UDP free. Missing historical rows have no numeric delta. No Beken resource inference. Compilation is not demonstrated CoAP feasibility.
