# Phase 2 test execution boundary — G3-A

G4 is not authorized. Do not flash, monitor hardware, run network campaigns, alter AP/firewall settings or claim acceptance from this document. G3-A validates syntax, dependency/configuration integrity, compilation and static ownership only.

The approved G2 matrix remains the future test scope: P2-01 identities; 02 missing private header; 03 primary build; 04 scoped build; 05 configuration/secret review; 06 server syntax/start/conflict; 07 valid server probe; 08 negative probes; 09 deployment/boot; 10 healthy 20/20; 11 independent reset; 12 packet capture; 13 dropped request; 14 dropped response; 15 duplicate request; 16 duplicate response; 17 separate response; 18 duplicate separate CON; 19 blackhole retry exhaustion; 20 empty ACK only; 21 wrong token; 22 wrong code/body; 23 malformed packet; 24 wrong port; 25 RST; 26 primary resources; 27 scoped resources; 28 lifecycle/tasks; 29 Beken portability.

Only P2-01/P2-03/P2-05 and the syntax portion of P2-06 have G3-A evidence where documented. All runtime and fault checks remain Not Executed. G3-C fixture is absent. Detailed execution setup, expected/actual logs, PASS/FAIL and captures must be recorded at the appropriate authorized stage. Final separate-CON ACK must be observed before cleanup; synchronous library ACK invocation alone does not prove delivery.
